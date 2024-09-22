# include guard
if (_zip2cpp_included)
    return()
endif (_zip2cpp_included)
set(_zip2cpp_included true)


# Zip files from directory into flat zip
# Jar and python generate bad quality archives, don't use it
macro(FlatZipDirectory curdir destination)
    get_filename_component(DESTINATION_PARENT_DIR ${destination} DIRECTORY ABSOLUTE)
    if (DESTINATION_PARENT_DIR)
        file(MAKE_DIRECTORY ${DESTINATION_PARENT_DIR})
    endif ()
    get_filename_component(PARENT_DIR ${curdir} DIRECTORY ABSOLUTE)
    file(GLOB_RECURSE filelist LIST_DIRECTORIES false RELATIVE ${PARENT_DIR} ${curdir}/*)
    find_program(ZIP_EXE NAMES zip)
    execute_process(COMMAND ${ZIP_EXE} --version OUTPUT_VARIABLE ZIP_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX MATCH "Zip ([0-9/.]*)" _ "${ZIP_VERSION}")
    set(ZIP_VERSION ${CMAKE_MATCH_1})
    find_program(TAR_EXE NAMES bsdtar tar)
    execute_process(COMMAND ${TAR_EXE} --version OUTPUT_VARIABLE TAR_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

    # bsdtar generates same output as 'cmake -E tar --format=zip' on linux
    # on windows cmake tar is not stable, so using windows build-in tar instead
    # by default use zip command if available, as it avoids generation of extra field
    if (ZIP_EXE AND ZIP_VERSION VERSION_GREATER_EQUAL 3.0)
        message("Using zip ${ZIP_VERSION} to archive assets")
        execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${PARENT_DIR} ${ZIP_EXE} -FSrq -D -X ${destination} ${filelist})
    elseif (TAR_EXE AND TAR_VERSION MATCHES "zlib")
        string(REGEX MATCH "bsdtar ([0-9/.]*)" _ "${TAR_VERSION}")
        set(TAR_VERSION ${CMAKE_MATCH_1})
        message("Zip not found. Using bsdtar ${TAR_VERSION} to archive assets")
        execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${PARENT_DIR} ${TAR_EXE} caf ${destination} ${filelist})
    else ()
        message("Zip or bsdtar not found. Using cmake ${CMAKE_VERSION} to archive assets")
        execute_process(COMMAND ${CMAKE_COMMAND} -E chdir ${PARENT_DIR} ${CMAKE_COMMAND} -E tar cf ${destination} --format=zip ${filelist})
    endif ()
endmacro()

# do the job
FlatZipDirectory(${INPUT} ${OUTPUT})
find_program(STRIP_EXE NAMES strip-nondeterminism)
if (STRIP_EXE)
    execute_process(COMMAND ${STRIP_EXE} ${OUTPUT})
    return()
endif ()

# based on https://github.com/KittyHawkCorp/stripzip/blob/d55bce7ead2711328e2867adad28c908add62a3a/src/stripzip_app.c
# generated with https://tomeko.net/online_tools/cpp_text_escape.php?lang=en
if (NOT EXISTS stripzip_app.c)
    file(WRITE stripzip_app.c
            "/**\n"
            " * @file\n"
            " * StripZIP\n"
            " * Sanitize a ZIP file from all horrible timestamps, UID, and GID nonsense.\n"
            " *\n"
            " * ZIP specification at https://pkware.cachefly.net/webdocs/casestudies/APPNOTE.TXT\n"
            " * Additional extended header information available from\n"
            " *   ftp://ftp.info-zip.org/pub/infozip/src/zip30.zip ./proginfo/extrafld.txt\n"
            " *\n"
            " * Copyright (c) 2016, Zee.Aero\n"
            " * All rights reserved.\n"
            " */\n"
            "\n"
            "#include <stdint.h>\n"
            "#include <stdio.h>\n"
            "#include <inttypes.h>\n"
            "#include <stdbool.h>\n"
            "#include <string.h>\n"
            "\n"
            "const uint32_t FILE_HEADER_SIGNATURE = 0x04034b50;\n"
            "const uint32_t CENDIR_HEADER_SIGNATURE = 0x02014b50;\n"
            "const uint32_t EO_CENDIR_HEADER_SIGNATURE = 0x06054b50;\n"
            "\n"
            "#define GPB_ENCRYPTION_MASK (0x1 << 0)\n"
            "#define GPB_METHOD_6_DETAIL (0x3 << 1)\n"
            "#define GPB_NOT_SEEKABLE (0x1 << 3)\n"
            "#define GPB_METHOD_8_ENH_DEFLATE (0x1 << 4)\n"
            "#define GPB_PATCH_DATA (0x1 << 5)\n"
            "#define GPB_STRONG_ENCRYPTION_MASK (0x1 << 6)\n"
            "#define GPB_UT8_ENCODING (0x1 << 11)\n"
            "#define GPB_CD_ENCRYPTED_MASK (0x1 << 13)\n"
            "const uint16_t GP_BIT_ENC_MARKERS = GPB_ENCRYPTION_MASK | GPB_STRONG_ENCRYPTION_MASK | GPB_CD_ENCRYPTED_MASK;\n"
            "const uint16_t GP_BIT_UNKNOWN_FLAG_MASK = ~(GPB_ENCRYPTION_MASK | GPB_METHOD_6_DETAIL | GPB_NOT_SEEKABLE | GPB_METHOD_8_ENH_DEFLATE | GPB_PATCH_DATA |\n"
            "                                            GPB_STRONG_ENCRYPTION_MASK | GPB_UT8_ENCODING | GPB_CD_ENCRYPTED_MASK);\n"
            "\n"
            "/** Header ID stripzip will use to replace undesired data.\n"
            " *  XXX: I hope nothing else uses this header for anything\n"
            " */\n"
            "#define STRIPZIP_OPTION_HEADER 0xFFFF\n"
            "#pragma pack(push, 1)\n"
            "typedef struct  {\n"
            "  uint32_t signature;\n"
            "  uint16_t version_needed;\n"
            "  uint16_t gp_bits;\n"
            "  uint16_t compression_method;\n"
            "  uint16_t last_mod_time;\n"
            "  uint16_t last_mod_date;\n"
            "  uint32_t crc32;\n"
            "  uint32_t compressed_size;\n"
            "  uint32_t uncompressed_size;\n"
            "  uint16_t name_length;\n"
            "  uint16_t extra_field_length;\n"
            "} local_file_header_t;\n"
            "\n"
            "typedef struct  {\n"
            "  uint32_t signature;\n"
            "  uint16_t version_made_by;\n"
            "  uint16_t version_needed;\n"
            "  uint16_t gp_bits;\n"
            "  uint16_t compression_method;\n"
            "  uint16_t last_mod_time;\n"
            "  uint16_t last_mod_date;\n"
            "  uint32_t crc32;\n"
            "  uint32_t compressed_size;\n"
            "  uint32_t uncompressed_size;\n"
            "  uint16_t file_name_length;\n"
            "  uint16_t extra_field_length;\n"
            "  uint16_t file_comment_length;\n"
            "  uint16_t disk_number_start;\n"
            "  uint16_t internal_attr;\n"
            "  uint32_t external_attr;\n"
            "  uint32_t rel_offset_local_header;\n"
            "} central_directory_header_t;\n"
            "\n"
            "typedef struct  {\n"
            "  uint32_t signature;\n"
            "  uint16_t disk_number;\n"
            "  uint16_t disk_num_start_of_cd;\n"
            "  uint16_t num_dir_entries_this_disk;\n"
            "  uint16_t total_num_entries_cd;\n"
            "  uint32_t size_of_cd;\n"
            "  uint32_t cd_offset_in_first_disk;\n"
            "  uint16_t zip_file_comment_length;\n"
            "} end_of_central_directory_header_t;\n"
            "\n"
            "typedef struct  {\n"
            "  uint16_t id;\n"
            "  uint16_t length;\n"
            "} extra_header_t;\n"
            "#pragma pack(pop)\n"
            "/**\n"
            " * Convenience function to overwrite a section of data with the file seek at\n"
            " * the end of the segment, and then set the seek back to the segment end.\n"
            " */\n"
            "void overwrite_field(void* data, size_t len, FILE* fd) {\n"
            "  fseek(fd, -1 * len, SEEK_CUR);\n"
            "  fwrite(data, len, 1, fd);\n"
            "}\n"
            "\n"
            "/**\n"
            " * Take either a central directory or local file extra data field and for the\n"
            " * things we know are horrible; purify it!\n"
            " *\n"
            " * TODO: It would be better if stripzip removed the headers completely so that the\n"
            " * ZIP was invariant regardless of what crazy program created it. But that's hard.\n"
            " */\n"
            "bool purify_extra_data(size_t len, void* extra_data) {\n"
            "  size_t offset = 0;\n"
            "  while (offset < len) {\n"
            "    extra_header_t* hdr = (extra_header_t*)extra_data + offset;\n"
            "    offset += sizeof(extra_header_t);\n"
            "\n"
            "    switch (hdr->id) {\n"
            "      case 0x5455:\n"
            "        /* Some sort of extended time data, see\n"
            "         * ftp://ftp.info-zip.org/pub/infozip/src/zip30.zip ./proginfo/extrafld.txt\n"
            "        .. fallthrough */\n"
            "      case 0x7875:\n"
            "        /* Unix extra data; UID / GID stuff, see\n"
            "         * ftp://ftp.info-zip.org/pub/infozip/src/zip30.zip ./proginfo/extrafld.txt\n"
            "         */\n"
            "        hdr->id = STRIPZIP_OPTION_HEADER;\n"
            "        memset((extra_header_t*)extra_data + offset, 0xFF, hdr->length);\n"
            "        break;\n"
            "\n"
            "      case STRIPZIP_OPTION_HEADER:\n"
            "        break;\n"
            "\n"
            "      default:\n"
            "        //printf(\"\\tUnknown extra header: 0x%x %u\\n\", hdr->id, hdr->length);\n"
            "        return false;\n"
            "        break;\n"
            "    }\n"
            "    offset += hdr->length;\n"
            "  }\n"
            "\n"
            "  return true;\n"
            "}\n"
            "\n"
            "int main(int argc, char** argv) {\n"
            "  if (argc != 2) {\n"
            "    printf(\"Usage: stripzip <in.zip>\\n\");\n"
            "    return -1;\n"
            "  }\n"
            "\n"
            "  FILE* zf = NULL;\n"
            "  zf = fopen(argv[1], \"rb+\");\n"
            "\n"
            "  /* Get the EO CenDir header */\n"
            "  fseek(zf, -sizeof(end_of_central_directory_header_t), SEEK_END);\n"
            "  end_of_central_directory_header_t eocd_header;\n"
            "  fread(&eocd_header, sizeof(eocd_header), 1, zf);\n"
            "  if (eocd_header.signature != EO_CENDIR_HEADER_SIGNATURE) {\n"
            "    printf(\"Did not get a good end of directory header! There might be a ZIP file comment?\\n\");\n"
            "    return -1;\n"
            "  }\n"
            "  if (eocd_header.disk_number != 0) {\n"
            "    printf(\"Split archive! This tool doesn't deal with those!\\n\");\n"
            "    return -1;\n"
            "  }\n"
            "  if (eocd_header.size_of_cd == 0xFFFFFFFF) {\n"
            "    printf(\"This is a Zip64 file; and I don't know how to deal with those!\\n\");\n"
            "    return -1;\n"
            "  }\n"
            "\n"
            "  /* For each entry in the central directory; purify it! */\n"
            "  char local_filename[UINT16_MAX];\n"
            "  char local_filecomment[UINT16_MAX];\n"
            "  char local_extra[UINT16_MAX];\n"
            "  fseek(zf, eocd_header.cd_offset_in_first_disk, SEEK_SET);\n"
            "  size_t dir_entry;\n"
            "  for (dir_entry = 0; dir_entry < eocd_header.total_num_entries_cd; dir_entry++) {\n"
            "    //printf(\"Now purifying entry %lu / %u (offset 0x08%lx) \", dir_entry + 1, eocd_header.total_num_entries_cd, ftell(zf));\n"
            "\n"
            "    central_directory_header_t cd_header = {0};\n"
            "    fread(&cd_header, sizeof(cd_header), 1, zf);\n"
            "    {\n"
            "      if (cd_header.signature != CENDIR_HEADER_SIGNATURE) {\n"
            "        printf(\"File corrupted! Central directory signature bad (0x%x).\\n\", cd_header.signature);\n"
            "        return -1;\n"
            "      }\n"
            "\n"
            "      if ((cd_header.gp_bits & GP_BIT_ENC_MARKERS) != 0x0) {\n"
            "        printf(\"Entry encrypted, I don't know how to deal with that.\\n\");\n"
            "        return -1;\n"
            "      }\n"
            "      if ((cd_header.gp_bits & GP_BIT_UNKNOWN_FLAG_MASK) != 0) {\n"
            "        printf(\"Entry has strange general purpose bits: %u\\n\", cd_header.gp_bits);\n"
            "        return -1;\n"
            "      }\n"
            "\n"
            "      // Purify time / date of CD header\n"
            "      cd_header.last_mod_date = 33;\n"
            "      cd_header.last_mod_time = 24608;\n"
            "      overwrite_field(&cd_header, sizeof(cd_header), zf);\n"
            "\n"
            "      // Get the file name and comment\n"
            "      fread(local_filename, cd_header.file_name_length, 1, zf);\n"
            "      if (cd_header.file_comment_length > 0) {\n"
            "        fread(local_filecomment, cd_header.file_comment_length, 1, zf);\n"
            "      }\n"
            "      //printf(\"%.*s\\n\", cd_header.file_name_length, local_filename);\n"
            "    }\n"
            "\n"
            "    // Get and purify the extra data\n"
            "    if (cd_header.extra_field_length) {\n"
            "      fread(local_extra, cd_header.extra_field_length, 1, zf);\n"
            "      purify_extra_data(cd_header.extra_field_length, local_extra);\n"
            "      overwrite_field(local_extra, cd_header.extra_field_length, zf);\n"
            "    }\n"
            "\n"
            "    // Now deal with the local header\n"
            "    size_t current_cd_position = ftell(zf);\n"
            "    fseek(zf, cd_header.rel_offset_local_header, SEEK_SET);\n"
            "    {\n"
            "      local_file_header_t lf_header = {0};\n"
            "\n"
            "      fread(&lf_header, sizeof(local_file_header_t), 1, zf);\n"
            "\n"
            "      if ((lf_header.gp_bits & GP_BIT_ENC_MARKERS) != 0x0) {\n"
            "        printf(\"Entry encrypted, I don't know how to deal with that.\\n\");\n"
            "        return -1;\n"
            "      }\n"
            "      if ((lf_header.gp_bits & GP_BIT_UNKNOWN_FLAG_MASK) != 0) {\n"
            "        printf(\"Entry has strange general purpose bits: %u\\n\", cd_header.gp_bits);\n"
            "        return -1;\n"
            "      }\n"
            "\n"
            "      lf_header.last_mod_date = 33;\n"
            "      lf_header.last_mod_time = 24608;\n"
            "      overwrite_field(&lf_header, sizeof(local_file_header_t), zf);\n"
            "\n"
            "      // Seek over the filename (assuming there's nothing sensitive in here)\n"
            "      fseek(zf, lf_header.name_length, SEEK_CUR);\n"
            "\n"
            "      // Take care of local data extra\n"
            "      if (lf_header.extra_field_length) {\n"
            "        fread(local_extra, lf_header.extra_field_length, 1, zf);\n"
            "        purify_extra_data(lf_header.extra_field_length, local_extra);\n"
            "        overwrite_field(local_extra, lf_header.extra_field_length, zf);\n"
            "      }\n"
            "    }\n"
            "    fseek(zf, current_cd_position, SEEK_SET);\n"
            "  }\n"
            "\n"
            "  fclose(zf);\n"
            "  return 0;\n"
            "}\n"
            ""
    )
endif ()

if (NOT C_COMPILER)
    find_program(C_COMPILER NAMES cc clang gcc cl icc bcc32 bcc xlc PATHS ENV PATH NO_DEFAULT_PATH)
endif ()
if (NOT C_COMPILER)
    message(FATAL_ERROR "C compiler not found manually")
endif ()
if (NOT EXISTS stripzip_app AND NOT EXISTS stripzip_app.exe)
    execute_process(COMMAND ${C_COMPILER} stripzip_app.c -o stripzip_app)
endif ()

get_filename_component(FILE_NAME ${OUTPUT} ABSOLUTE)
execute_process(COMMAND ${CMAKE_CURRENT_BINARY_DIR}/stripzip_app ${FILE_NAME})
