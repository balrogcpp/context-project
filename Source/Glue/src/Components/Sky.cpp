// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Components/Sky.h"
#include "Components/ArHosekSkyModelData_RGB.h"
#include "Log.h"
#include "Components/Scene.h"
#include "Engine.h"
#include "Caelum.h"
#include <iostream>
extern "C" {
#define _USE_MATH_DEFINES
#include <math.h>
}

using namespace std;
using namespace Ogre;

static const float TURBIDITY = 4.0f;
static const float ALBEDO[] = {0.1, 0.1, 0.1};

static Vector3 pow(Vector3 a, Vector3 b) {
  return Vector3(pow(a.x, b.x), pow(a.y, b.y), pow(a.z, b.z));
}

static Vector3 exp(Vector3 a) {
  return Vector3(exp(a.x), exp(a.x), exp(a.z));
}

static Vector3 mul_element_wise(Vector3 a, Vector3 b) {
  return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
}

namespace Glue {

template <typename T, size_t N>
void PrintParams(const array<T, N> params) {
  Log::Message("===================================================================================");
  for (const auto &it : params) Log::Message(to_string(it));
  Log::Message("===================================================================================");
}

static Vector3 hosek_wilkie(float cos_theta, float gamma, float cos_gamma, const array<Vector3, 10> &params) {
  Vector3 A = params[0];
  Vector3 B = params[1];
  Vector3 C = params[2];
  Vector3 D = params[3];
  Vector3 E = params[4];
  Vector3 F = params[5];
  Vector3 G = params[6];
  Vector3 H = params[7];
  Vector3 I = params[8];

  Vector3 chi = (1.0 + cos_gamma * cos_gamma) / pow(1.0 + H * H - 2.0 * cos_gamma * H, Vector3(1.5));
  return (1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * sqrt(cos_theta));
}

static float evaluate_spline(float *dataset, size_t start, size_t stride, float value) {
  return 1.0 *  pow(1.0 - value, 5) *                 dataset[start + 0 * stride] +
         5.0 *  pow(1.0 - value, 4) * pow(value, 1) * dataset[start + 1 * stride] +
         10.0 * pow(1.0 - value, 3) * pow(value, 2) * dataset[start + 2 * stride] +
         10.0 * pow(1.0 - value, 2) * pow(value, 3) * dataset[start + 3 * stride] +
         5.0 *  pow(1.0 - value, 1) * pow(value, 4) * dataset[start + 4 * stride] +
         1.0 *                        pow(value, 5) * dataset[start + 5 * stride];
}

static float evaluate(float *dataset, size_t stride, float turbidity, float albedo, float sun_theta) {
// splines are functions of elevation^1/3
  float elevationK = pow(max(1.0 - sun_theta / M_PI_2, 0.0), 1.0 / 3.0);

  // table has values for turbidity 1..10
  float turbidity0 = clamp(turbidity, 1.0f, 10.0f);
  float turbidity1 = min(turbidity0 + 1.0f, 10.0f);
  float turbidityK = clamp(turbidity - turbidity0, 0.0f, 1.0f);

  size_t datasetA0 = 0;
  size_t datasetA1 = stride * 6 * 10;

  float a0t0 = evaluate_spline(dataset, datasetA0 + stride * 6 * (turbidity0 - 1), stride, elevationK);
  float a1t0 = evaluate_spline(dataset, datasetA1 + stride * 6 * (turbidity0 - 1), stride, elevationK);
  float a0t1 = evaluate_spline(dataset, datasetA0 + stride * 6 * (turbidity1 - 1), stride, elevationK);
  float a1t1 = evaluate_spline(dataset, datasetA1 + stride * 6 * (turbidity1 - 1), stride, elevationK);

  return a0t0 * (1.0 - albedo) * (1.0 - turbidityK) + a1t0 * albedo * (1.0 - turbidityK) + a0t1 * (1.0 - albedo) * turbidityK + a1t1 * albedo * turbidityK;
}

array<Vector3, 10> recalc_sun(Vector3 sun_dir) {
  float sun_theta = acos(clamp(sun_dir.y, 0.0f, 1.0f));

  array<Vector3, 10> params;
  for (int i = 0; i < 3; i++) {
    params[0][i] = evaluate(datasetsRGB[i], 9, TURBIDITY, ALBEDO[i], sun_theta);
    params[1][i] = evaluate(datasetsRGB[i], 9, TURBIDITY, ALBEDO[i], sun_theta);
    params[2][i] = evaluate(datasetsRGB[i], 9, TURBIDITY, ALBEDO[i], sun_theta);
    params[3][i] = evaluate(datasetsRGB[i], 9, TURBIDITY, ALBEDO[i], sun_theta);
    params[4][i] = evaluate(datasetsRGB[i], 9, TURBIDITY, ALBEDO[i], sun_theta);
    params[5][i] = evaluate(datasetsRGB[i], 9, TURBIDITY, ALBEDO[i], sun_theta);
    params[6][i] = evaluate(datasetsRGB[i], 9, TURBIDITY, ALBEDO[i], sun_theta);

    params[7][i] = evaluate(datasetsRGB[i], 9, TURBIDITY, ALBEDO[i], sun_theta);
    params[8][i] = evaluate(datasetsRGB[i], 9, TURBIDITY, ALBEDO[i], sun_theta);

// Z value thing
    params[9][i] = evaluate(datasetsRGBRad[i], 1, TURBIDITY, ALBEDO[i], sun_theta);
  }

  Vector3 S = mul_element_wise(hosek_wilkie(cos(sun_theta), 0.0, 1.0, params), params[9]);
  params[9] /= S.dotProduct(Vector3(0.2126, 0.7152, 0.0722));

  float sun_amount = remainder(sun_dir.y / M_PI_2, 4.0f);
  if (sun_amount > 2.0) {
    sun_amount = 0.0;//-(sun_amount - 2.0);
  }
  if (sun_amount > 1.0) {
    sun_amount = 2.0 - sun_amount;
  } else if (sun_amount < -1.0) {
    sun_amount = -2.0 - sun_amount;
  }

  float normalized_sun_y = 0.6 + 0.45 * sun_amount;
  params[9] *= normalized_sun_y;

  return params;
}

Sky::Sky() {}

Sky::~Sky() {}

void Sky::OnSetUp() {
  Vector3 SunPosition = -GetScene().GetSunPosition();
  PrintParams(recalc_sun(SunPosition.normalisedCopy()));
}

void Sky::OnPause() {
  Paused = true;
  // Add your code
}

void Sky::OnResume() {
  Paused = false;
  // Add your code
}

void Sky::OnUpdate(float TimePassed) {
  // Add your code
}

void Sky::OnClean() {
  // Add your code
}

}  // namespace Glue
