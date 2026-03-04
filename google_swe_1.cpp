#include <cstdint>
#include <cstdio>

/**
 * Your objective is to print out all record values, skipping any duplicate record values that exist.
 * 
 * You have a machine with the following limitations:
 * 
 * - You have 10kB (10240 bytes) of CPU RAM available.
 * 
 * You have access to external memory emCC in order to read/write more memory as you wish.
 * 
 * Here are the APIs you have access to:
 * 
 */

#define RESULT_OK 0
#define RESULT_OUT_OF_MEMORY 1
#define RESULT_FAIL 2

extern void getRecord(uint64_t recordId, uint16_t* recordValue);
extern uint64_t getNumRecords();

extern void emCC_write(uint64_t address, uint16_t* data, uint64_t size);
extern void emCC_read (uint64_t address, uint16_t* buffer, uint64_t size);


int main()
{
    uint64_t BUCKET_SIZE = 1024;
    uint64_t numRecords = getNumRecords();
    uint64_t buckets[BUCKET_SIZE] = {0};

    uint16_t recordValue;

    uint64_t i;

    // Set the correct bits in each bucket
    for (i = 0; i < numRecords; ++i) {
        getRecord(i, &recordValue);
        buckets[recordValue/BUCKET_SIZE] |= 1 << (recordValue % BUCKET_SIZE);
    }

    // Print the unique record values
    for (i = 0; i < BUCKET_SIZE; ++i) {
        uint64_t bucket = buckets[i];
        for (uint64_t j = 0; j < BUCKET_SIZE; ++j) {
            if (bucket & (1 << j)) {
                printf("%d", i * BUCKET_SIZE + j);
            }
        }
    }
}