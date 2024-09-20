#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>

const uint32_t FILE_HEADER_SIGNATURE = 0x04034b50;
const uint32_t CENDIR_HEADER_SIGNATURE = 0x02014b50;
const uint32_t EO_CENDIR_HEADER_SIGNATURE = 0x06054b50;

#define GPB_ENCRYPTION_MASK (0x1 << 0)
#define GPB_METHOD_6_DETAIL (0x3 << 1)
#define GPB_NOT_SEEKABLE (0x1 << 3)
#define GPB_METHOD_8_ENH_DEFLATE (0x1 << 4)
#define GPB_PATCH_DATA (0x1 << 5)
#define GPB_STRONG_ENCRYPTION_MASK (0x1 << 6)
#define GPB_UT8_ENCODING (0x1 << 11)
#define GPB_CD_ENCRYPTED_MASK (0x1 << 13)
const uint16_t GP_BIT_ENC_MARKERS = GPB_ENCRYPTION_MASK | GPB_STRONG_ENCRYPTION_MASK | GPB_CD_ENCRYPTED_MASK;
const uint16_t GP_BIT_UNKNOWN_FLAG_MASK = ~(GPB_ENCRYPTION_MASK | GPB_METHOD_6_DETAIL | GPB_NOT_SEEKABLE | GPB_METHOD_8_ENH_DEFLATE | GPB_PATCH_DATA |
                                            GPB_STRONG_ENCRYPTION_MASK | GPB_UT8_ENCODING | GPB_CD_ENCRYPTED_MASK);

#define STRIPZIP_OPTION_HEADER 0xFFFF
#pragma pack(push, 1)
typedef struct {
  uint32_t signature;
  uint16_t version_needed;
  uint16_t gp_bits;
  uint16_t compression_method;
  uint16_t last_mod_time;
  uint16_t last_mod_date;
  uint32_t crc32;
  uint32_t compressed_size;
  uint32_t uncompressed_size;
  uint16_t name_length;
  uint16_t extra_field_length;
} local_file_header_t;

typedef struct {
  uint32_t signature;
  uint16_t version_made_by;
  uint16_t version_needed;
  uint16_t gp_bits;
  uint16_t compression_method;
  uint16_t last_mod_time;
  uint16_t last_mod_date;
  uint32_t crc32;
  uint32_t compressed_size;
  uint32_t uncompressed_size;
  uint16_t file_name_length;
  uint16_t extra_field_length;
  uint16_t file_comment_length;
  uint16_t disk_number_start;
  uint16_t internal_attr;
  uint32_t external_attr;
  uint32_t rel_offset_local_header;
} central_directory_header_t;

typedef struct {
  uint32_t signature;
  uint16_t disk_number;
  uint16_t disk_num_start_of_cd;
  uint16_t num_dir_entries_this_disk;
  uint16_t total_num_entries_cd;
  uint32_t size_of_cd;
  uint32_t cd_offset_in_first_disk;
  uint16_t zip_file_comment_length;
} end_of_central_directory_header_t;

typedef struct {
  uint16_t id;
  uint16_t length;
} extra_header_t;
#pragma pack(pop)

void overwrite_field(void* data, size_t len, std::fstream& fd) {
  fd.seekp(-len, std::ios::cur);
  fd.write(reinterpret_cast<char*>(data), len);
}

bool purify_extra_data(size_t len, void* extra_data) {
  size_t offset = 0;
  while (offset < len) {
    extra_header_t* hdr = (extra_header_t*)extra_data + offset;
    offset += sizeof(extra_header_t);

    switch (hdr->id) {
      case 0x5455:
      case 0x7875:
        hdr->id = STRIPZIP_OPTION_HEADER;
        memset((extra_header_t*)extra_data + offset, 0xFF, hdr->length);
        break;

      case STRIPZIP_OPTION_HEADER:
        break;

      default:
        std::cout << "\tUnknown extra header: 0x" << std::hex << hdr->id << " " << hdr->length << std::endl;
        return false;
    }
    offset += hdr->length;
  }

  return true;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: stripzip <in.zip>" << std::endl;
    return -1;
  }

  std::fstream zf(argv[1], std::ios::in | std::ios::out | std::ios::binary);
  if (!zf.is_open()) {
    std::cerr << "Failed to open file." << std::endl;
    return -1;
  }

  zf.seekg(-sizeof(end_of_central_directory_header_t), std::ios::end);
  end_of_central_directory_header_t eocd_header;
  zf.read(reinterpret_cast<char*>(&eocd_header), sizeof(eocd_header));
  if (eocd_header.signature != EO_CENDIR_HEADER_SIGNATURE) {
    std::cout << "Did not get a good end of directory header! There might be a ZIP file comment?" << std::endl;
    return -1;
  }
  if (eocd_header.disk_number != 0) {
    std::cout << "Split archive! This tool doesn't deal with those!" << std::endl;
    return -1;
  }
  if (eocd_header.size_of_cd == 0xFFFFFFFF) {
    std::cout << "This is a Zip64 file; and I don't know how to deal with those!" << std::endl;
    return -1;
  }

  char local_filename[UINT16_MAX];
  char local_filecomment[UINT16_MAX];
  char local_extra[UINT16_MAX];
  zf.seekg(eocd_header.cd_offset_in_first_disk, std::ios::beg);
  size_t dir_entry;
  for (dir_entry = 0; dir_entry < eocd_header.total_num_entries_cd; dir_entry++) {
    std::cout << "Now purifying entry " << dir_entry + 1 << " / " << eocd_header.total_num_entries_cd << " (offset 0x08" << std::hex << zf.tellg()
              << ") ";

    central_directory_header_t cd_header = {0};
    zf.read(reinterpret_cast<char*>(&cd_header), sizeof(cd_header));
    {
      if (cd_header.signature != CENDIR_HEADER_SIGNATURE) {
        std::cout << "File corrupted! Central directory signature bad (0x" << std::hex << cd_header.signature << ")." << std::endl;
        return -1;
      }

      if ((cd_header.gp_bits & GP_BIT_ENC_MARKERS) != 0x0) {
        std::cout << "Entry encrypted, I don't know how to deal with that." << std::endl;
        return -1;
      }
      if ((cd_header.gp_bits & GP_BIT_UNKNOWN_FLAG_MASK) != 0) {
        std::cout << "Entry has strange general purpose bits: " << cd_header.gp_bits << std::endl;
        return -1;
      }

      cd_header.last_mod_date = 33;
      cd_header.last_mod_time = 24608;
      overwrite_field(&cd_header, sizeof(cd_header), zf);

      zf.read(local_filename, cd_header.file_name_length);
      if (cd_header.file_comment_length > 0) {
        zf.read(local_filecomment, cd_header.file_comment_length);
      }
      std::cout << std::string(local_filename, cd_header.file_name_length) << std::endl;
    }

    if (cd_header.extra_field_length) {
      zf.read(local_extra, cd_header.extra_field_length);
      purify_extra_data(cd_header.extra_field_length, local_extra);
      overwrite_field(local_extra, cd_header.extra_field_length, zf);
    }

    size_t current_cd_position = zf.tellg();
    zf.seekg(cd_header.rel_offset_local_header, std::ios::beg);
    {
      local_file_header_t lf_header = {0};

      zf.read(reinterpret_cast<char*>(&lf_header), sizeof(local_file_header_t));

      if ((lf_header.gp_bits & GP_BIT_ENC_MARKERS) != 0x0) {
        std::cout << "Entry encrypted, I don't know how to deal with that." << std::endl;
        return -1;
      }
      if ((lf_header.gp_bits & GP_BIT_UNKNOWN_FLAG_MASK) != 0) {
        std::cout << "Entry has strange general purpose bits: " << cd_header.gp_bits << std::endl;
        return -1;
      }

      lf_header.last_mod_date = 33;
      lf_header.last_mod_time = 24608;
      overwrite_field(&lf_header, sizeof(local_file_header_t), zf);

      zf.seekg(lf_header.name_length, std::ios::cur);

      if (lf_header.extra_field_length) {
        zf.read(local_extra, lf_header.extra_field_length);
        purify_extra_data(lf_header.extra_field_length, local_extra);
        overwrite_field(local_extra, lf_header.extra_field_length, zf);
      }
    }
    zf.seekg(current_cd_position, std::ios::beg);
  }

  zf.close();
  return 0;
}
