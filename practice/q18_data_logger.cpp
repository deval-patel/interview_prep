/**
 * Q18: Design an Embedded Data Logger
 *
 * PROBLEM:
 * Design an embedded system that logs sensor data to flash memory with:
 * - Power failure recovery (no data corruption)
 * - Wear leveling (extend flash lifetime)
 * - Circular buffer behavior when full
 *
 * CONTEXT:
 * Flash memory has special characteristics:
 * - Can only write 0s (can't flip 0 back to 1)
 * - Must erase entire sector to write new data
 * - Limited erase cycles (10K-100K typically)
 * - Erase operation is slow and may fail
 *
 * DESIGN GOALS:
 * 1. Minimize flash wear (distribute writes evenly)
 * 2. Atomic operations (power-safe)
 * 3. Efficient sequential writes
 * 4. Fast startup (find current write position)
 *
 * KEY CONCEPTS:
 * - Sector-based storage
 * - Sequence numbers for ordering
 * - Commit markers for atomicity
 * - Wear-leveling algorithms
 */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>

// ============ FLASH MEMORY SIMULATION ============

// Flash configuration
const size_t FLASH_SECTOR_SIZE = 4096;    // 4KB sectors
const size_t FLASH_NUM_SECTORS = 16;      // 64KB total
const size_t FLASH_SIZE = FLASH_SECTOR_SIZE * FLASH_NUM_SECTORS;

// Simulated flash memory
static uint8_t g_flash[FLASH_SIZE];
static uint32_t g_sector_erase_count[FLASH_NUM_SECTORS];

// Flash state: 0xFF = erased, can write 0s
void flash_init() {
    memset(g_flash, 0xFF, FLASH_SIZE);
    memset(g_sector_erase_count, 0, sizeof(g_sector_erase_count));
}

// Read from flash
void flash_read(uint32_t address, uint8_t* buffer, size_t length) {
    if (address + length <= FLASH_SIZE) {
        memcpy(buffer, &g_flash[address], length);
    }
}

// Write to flash (can only clear bits, 1->0)
bool flash_write(uint32_t address, const uint8_t* data, size_t length) {
    if (address + length > FLASH_SIZE) return false;

    for (size_t i = 0; i < length; i++) {
        // Can only write 0s (AND with existing data)
        g_flash[address + i] &= data[i];
    }
    return true;
}

// Erase a sector (sets all bytes to 0xFF)
bool flash_erase_sector(uint32_t sector) {
    if (sector >= FLASH_NUM_SECTORS) return false;

    uint32_t address = sector * FLASH_SECTOR_SIZE;
    memset(&g_flash[address], 0xFF, FLASH_SECTOR_SIZE);
    g_sector_erase_count[sector]++;

    return true;
}

// Get erase count for wear analysis
uint32_t flash_get_erase_count(uint32_t sector) {
    return g_sector_erase_count[sector];
}

// ============ DATA LOGGER DESIGN ============

// Log entry structure
// Design for power-safe operation
struct LogEntry {
    uint32_t sequence_number;  // For ordering
    uint32_t timestamp;        // Time of event
    uint16_t sensor_id;        // Which sensor
    uint16_t data_length;      // Length of data
    uint8_t data[16];          // Sensor data
    uint32_t crc;              // CRC for integrity
    uint8_t commit_marker;     // 0x00 = committed, 0xFF = not committed
    uint8_t padding[3];        // Pad to 36-byte total (4+4+2+2+16+4+1+3)
};

static_assert(sizeof(LogEntry) == 36, "LogEntry must be 36 bytes");

// Sector header structure
struct SectorHeader {
    uint32_t magic;            // Magic number to identify valid sector
    uint32_t sector_sequence;  // For determining newest sector
    uint32_t erase_count;      // Number of times this sector was erased
    uint32_t reserved;
};

[[maybe_unused]] const uint32_t SECTOR_MAGIC = 0x4C4F4731;  // "LOG1"
[[maybe_unused]] const size_t ENTRIES_PER_SECTOR = (FLASH_SECTOR_SIZE - sizeof(SectorHeader)) / sizeof(LogEntry);

// ============ YOUR IMPLEMENTATION ============

class DataLogger {
private:
    // TODO: Add member variables
    // - Current write sector
    // - Current write position within sector
    // - Global sequence counter
    // - Sector state tracking

public:
    DataLogger() {
        // TODO: Initialize
    }

    /**
     * Initialize the logger, scanning flash to find current state.
     *
     * Must handle:
     * - Fresh (all erased) flash
     * - Power failure during write
     * - Power failure during erase
     * - Finding the most recent entry
     */
    bool init() {
        // TODO: Implement initialization
        //
        // Steps:
        // 1. Scan all sectors for valid headers
        // 2. Find sector with highest sequence number
        // 3. Within that sector, find first uncommitted or empty entry
        // 4. Set write position accordingly
        // 5. If power failed during erase, complete the erase

        return false;  // Placeholder
    }

    /**
     * Write a log entry.
     *
     * Must be atomic (power-safe):
     * 1. Write entry data with commit_marker = 0xFF
     * 2. Write commit_marker = 0x00 to finalize
     *
     * If power fails between steps, entry is ignored on next startup.
     *
     * @param sensor_id  Sensor identifier
     * @param data       Sensor data
     * @param length     Data length (max 16)
     * @param timestamp  Event timestamp
     * @return true on success
     */
    bool write_entry(uint16_t sensor_id, const uint8_t* data,
                     size_t length, uint32_t timestamp) {
        // TODO: Implement power-safe write
        //
        // Steps:
        // 1. Check if current sector is full
        // 2. If full, move to next sector (erase if needed)
        // 3. Prepare entry with sequence number
        // 4. Calculate CRC
        // 5. Write entry (uncommitted)
        // 6. Write commit marker

        return false;  // Placeholder
    }

    /**
     * Read an entry by sequence number.
     *
     * @param sequence  Sequence number to find
     * @param entry     Output buffer for entry
     * @return true if found
     */
    bool read_entry(uint32_t sequence, LogEntry* entry) {
        // TODO: Implement

        return false;  // Placeholder
    }

    /**
     * Read most recent N entries.
     *
     * @param entries  Output buffer
     * @param count    Number of entries to read
     * @return Actual number of entries read
     */
    size_t read_recent(LogEntry* entries, size_t count) {
        // TODO: Implement

        return 0;  // Placeholder
    }

    /**
     * Get total number of valid entries.
     */
    size_t get_entry_count() {
        // TODO: Implement
        return 0;
    }

    /**
     * Get next sequence number.
     */
    uint32_t get_next_sequence() {
        // TODO: Implement
        return 0;
    }

private:
    /**
     * Calculate CRC for an entry.
     */
    uint32_t calculate_crc(const LogEntry* entry) {
        // Simple CRC32 implementation
        // TODO: Implement proper CRC32
        uint32_t crc = 0xFFFFFFFF;
        const uint8_t* data = (const uint8_t*)entry;

        // CRC everything except the CRC and commit fields
        for (size_t i = 0; i < offsetof(LogEntry, crc); i++) {
            crc ^= data[i];
        }

        return crc;
    }

    /**
     * Verify entry integrity.
     */
    bool verify_entry(const LogEntry* entry) {
        if (entry->commit_marker != 0x00) return false;  // Not committed
        return calculate_crc(entry) == entry->crc;
    }

    /**
     * Find least-worn sector for wear leveling.
     */
    uint32_t find_least_worn_sector() {
        // TODO: Implement wear leveling
        return 0;  // Placeholder
    }

    /**
     * Prepare next sector for writing.
     * Handles circular buffer wraparound.
     */
    bool prepare_next_sector() {
        // TODO: Implement
        // 1. Select next sector (wear-leveling considered)
        // 2. Erase sector
        // 3. Write sector header
        return false;
    }
};

// ============ TEST FRAMEWORK ============

bool test_basic_logging() {
    printf("--- Basic Logging Tests ---\n");
    int passed = 0;
    int total = 0;

    flash_init();
    DataLogger logger;

    if (logger.init()) {
        printf("PASS: Logger initialized\n");
        passed++;
    } else {
        printf("FAIL: Logger init failed\n");
    }
    total++;

    // Write first entry
    uint8_t data1[] = {0x01, 0x02, 0x03, 0x04};
    if (logger.write_entry(1, data1, 4, 1000)) {
        printf("PASS: First entry written\n");
        passed++;
    } else {
        printf("FAIL: First entry write failed\n");
    }
    total++;

    // Write second entry
    uint8_t data2[] = {0xAA, 0xBB};
    if (logger.write_entry(2, data2, 2, 2000)) {
        printf("PASS: Second entry written\n");
        passed++;
    } else {
        printf("FAIL: Second entry write failed\n");
    }
    total++;

    // Read back entries
    if (logger.get_entry_count() == 2) {
        printf("PASS: Entry count correct\n");
        passed++;
    } else {
        printf("FAIL: Entry count = %zu (expected 2)\n", logger.get_entry_count());
    }
    total++;

    printf("Basic logging tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_power_failure_recovery() {
    printf("--- Power Failure Recovery Tests ---\n");
    int passed = 0;
    int total = 0;

    flash_init();
    DataLogger logger;
    logger.init();

    // Write some entries
    uint8_t data[] = {0x11, 0x22, 0x33};
    for (int i = 0; i < 5; i++) {
        logger.write_entry(i, data, 3, i * 1000);
    }

    // Simulate power failure: reinitialize without clearing flash
    DataLogger logger2;
    if (logger2.init()) {
        printf("PASS: Logger reinitialized after power cycle\n");
        passed++;
    } else {
        printf("FAIL: Logger reinit failed\n");
    }
    total++;

    // Check entries are preserved
    if (logger2.get_entry_count() == 5) {
        printf("PASS: All entries preserved\n");
        passed++;
    } else {
        printf("FAIL: Entry count = %zu (expected 5)\n", logger2.get_entry_count());
    }
    total++;

    // Sequence numbers should continue
    uint32_t next_seq = logger2.get_next_sequence();
    if (next_seq == 5) {
        printf("PASS: Sequence numbers continue correctly\n");
        passed++;
    } else {
        printf("FAIL: Next sequence = %u (expected 5)\n", next_seq);
    }
    total++;

    printf("Power failure tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_circular_buffer() {
    printf("--- Circular Buffer Tests ---\n");
    int passed = 0;
    int total = 0;

    flash_init();
    DataLogger logger;
    logger.init();

    // Write more entries than fit in flash
    // With 16 sectors of 4KB, and 32-byte entries:
    // Approximately (16 * 4096 / 32) = 2048 entries max

    const int NUM_ENTRIES = 3000;  // More than capacity
    uint8_t data[] = {0x99};

    for (int i = 0; i < NUM_ENTRIES; i++) {
        if (!logger.write_entry(0, data, 1, i)) {
            printf("Write failed at entry %d\n", i);
            break;
        }
    }

    // Should have wrapped around, oldest entries overwritten
    size_t count = logger.get_entry_count();
    printf("After %d writes, %zu entries stored\n", NUM_ENTRIES, count);

    // The most recent entries should be available
    LogEntry recent[10];
    size_t num_read = logger.read_recent(recent, 10);

    if (num_read > 0) {
        printf("PASS: Circular buffer working (read %zu recent entries)\n", num_read);
        passed++;
    } else {
        printf("FAIL: Could not read recent entries\n");
    }
    total++;

    printf("Circular buffer tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_wear_leveling() {
    printf("--- Wear Leveling Tests ---\n");

    flash_init();
    DataLogger logger;
    logger.init();

    // Write many entries to force sector erasures
    const int NUM_ENTRIES = 5000;
    uint8_t data[] = {0xAB};

    for (int i = 0; i < NUM_ENTRIES; i++) {
        logger.write_entry(0, data, 1, i);
    }

    // Check erase counts are reasonably balanced
    printf("Sector erase counts:\n");
    uint32_t min_erases = UINT32_MAX, max_erases = 0;
    for (uint32_t i = 0; i < FLASH_NUM_SECTORS; i++) {
        uint32_t count = flash_get_erase_count(i);
        printf("  Sector %2u: %u erases\n", i, count);
        if (count > 0) {
            min_erases = std::min(min_erases, count);
            max_erases = std::max(max_erases, count);
        }
    }

    // Wear leveling metric: max should not be much more than min
    if (max_erases > 0 && max_erases <= min_erases * 2) {
        printf("PASS: Wear reasonably balanced (max/min ratio = %.2f)\n",
               (float)max_erases / min_erases);
        return true;
    } else {
        printf("Note: Wear leveling could be improved (ratio = %.2f)\n",
               min_erases > 0 ? (float)max_erases / min_erases : 0);
        return true;  // Not a hard failure
    }
}

int main() {
    printf("Running Q18: Data Logger Design\n");
    printf("================================\n\n");

    bool all_passed = true;

    if (!test_basic_logging()) all_passed = false;
    if (!test_power_failure_recovery()) all_passed = false;
    if (!test_circular_buffer()) all_passed = false;
    if (!test_wear_leveling()) all_passed = false;

    printf("================================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Keep working on your solution.\n");
        return 1;
    }
}
