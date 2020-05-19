#include <iostream>
#include <cstring>
#include <cinttypes>
#include "PrefixTree.hpp"
#include "Stack.hpp"

void CountFreqs(FILE *fp, uint64_t *freqs) {
  memset(freqs, 0, sizeof(uint64_t) * 256);
  unsigned char buffer[4096];
  size_t got;
  while ((got = fread(buffer, 1, 4096, fp)) > 0) {
    for (int i = 0; i < got; ++i) {
      ++freqs[buffer[i]];
    }
  }
}

// takes mapping from char to codes and encodes text
class Encoder {
 public:
  Encoder(Code *codes) : codes_(codes) {}

  void EncodeFromFile(FILE *fp, FILE *out) {
    fseek(fp, 0, SEEK_SET);
    unsigned char buffer[4096];
    size_t got = fread(buffer, 1, 4096, fp);
    // current index in result
    size_t block = 0;
    // current position in block (in bits)
    size_t position = 0;
    static const size_t block_size = 32;
    static const size_t code_block_size = 8;
    uint32_t result[4096 * 8];
    memset(result, 0, 4096 * 32);
    while (got > 0) {
      for (size_t i = 0; i < got; ++i) {
        auto &next_code = codes_[buffer[i]];
        size_t code_size = next_code.GetSize();

        // current position in block (in bits) in code
        size_t code_position = 0;
        // current index in code
        size_t code_block = 0;


        while (code_block * code_block_size + code_position < code_size) {
          // left till the end of the block or the end of the code in code
          size_t code_complement = std::min(code_block_size - code_position,
                                            code_size - code_position - code_block * code_block_size);

          // left till the end of the block in result
          size_t result_complement = block_size - position;

          size_t to_write = std::min(code_complement, result_complement);

          unsigned char value = next_code.GetBlock(code_block);

          // move real code to the least significant bytes
          value >>= (code_block_size - (code_position + to_write));
          // now move to position corresponding to result
          uint32_t new_value = value;
          new_value <<= (block_size - (position + to_write));
          result[block] |= new_value;

          position += to_write;
          if (position == block_size) {
            position = 0;
            ++block;
          }
          code_position += to_write;
          if (code_position == code_block_size) {
            code_position = 0;
            ++code_block;
          }
        }
      }
      fwrite(result, 4, block, out);
      uint32_t last = result[block];
      memset(result, 0, 4096 * 32);
      result[0] = last;
      block = 0;
      got = fread(buffer, 1, 4096, fp);
    }
    fwrite(result, 4, 1, out);
  }

 private:
  Code *codes_;
};

int main(int argc, char *argv[]) {
  if (strcmp(argv[1], "encode") == 0) {
    for (int i = 2; i < argc; ++i) {
      char *filename = argv[i];
      std::string name(filename);
      FILE *fp = fopen(filename, "rb");
      fseek(fp, 0L, SEEK_END);
      size_t file_size = ftell(fp);
      fseek(fp, 0L, SEEK_SET);

      FILE *out = fopen((name + "_comp").c_str(), "wb");
      uint64_t freqs[256];
      CountFreqs(fp, freqs);
      PrefixTree tree(freqs);
      tree.BuildTree();
      tree.FindCodes();
      Encoder enc(tree.GetCodes());

      unsigned char symbols_order[256];


      Code code(2 * 256);
      tree.WriteTree(code, symbols_order);
      size_t code_size = code.GetSize();
      size_t to_write = code_size / 8;
      if (code_size % 8) ++to_write;

      fwrite(&code_size, 4, 1, out);
      fwrite(symbols_order, 1, (code_size >> 1u) + 1, out);
      fwrite(code.GetBlockPtr(0), 1, to_write, out);
      fwrite(&file_size, 4, 1, out);

      enc.EncodeFromFile(fp, out);
      fclose(fp);
      fclose(out);
    }
  } else if (strcmp(argv[1], "decode") == 0) {
    for (int i = 2; i < argc; ++i) {
      char *filename = argv[i];
      std::string name(filename);
      FILE *fp = fopen(filename, "rb");
      FILE *out = fopen((name + "orig").c_str(), "wb");

      PrefixTree tree;

      tree.ReadTree(fp);
      tree.Decode(fp, out);
      fclose(fp);
      fclose(out);
    }
  }
  return 0;
}
