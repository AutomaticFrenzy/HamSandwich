#ifndef LIBEXTRACT_INNO_H
#define LIBEXTRACT_INNO_H

#include <vector>
#include <string>
#include <map>
#include <stdio.h>
#include <stdint.h>
#include "common.h"

struct SDL_RWops;

namespace inno {

class Archive : public sauce::Archive
{
	struct DataEntry
	{
		uint32_t chunk_offset;
		uint64_t file_size;
		uint64_t chunk_size;
	};
	std::vector<DataEntry> data_entries;
	std::vector<uint8_t> setup_1_bin;

public:
	explicit Archive(FILE* fptr);
	~Archive();

	bool is_ok() { return setup_1_bin.size(); }
	SDL_RWops* open_file(const char* path);
};

}  // namespace inno

#endif  // LIBEXTRACT_INNO_H
