/**
 * Q19: Design an OTA Firmware Update System
 *
 * PROBLEM:
 * Design an over-the-air firmware update system for an IoT device with:
 * - Rollback capability (revert to previous firmware on failure)
 * - Verification (ensure firmware integrity before activating)
 * - Minimal downtime
 *
 * DESIGN REQUIREMENTS:
 * 1. Power failure safe at any point
 * 2. Verification before activation (CRC, signature)
 * 3. Automatic rollback on boot failure
 * 4. Efficient storage usage
 *
 * TYPICAL ARCHITECTURE:
 * Flash Layout:
 * +------------------+
 * | Bootloader       |  Fixed, rarely updated
 * +------------------+
 * | Boot config      |  Which slot to boot, boot count, flags
 * +------------------+
 * | Slot A (Active)  |  Current running firmware
 * +------------------+
 * | Slot B (Update)  |  Download new firmware here
 * +------------------+
 * | Persistent Data  |  User settings, logs
 * +------------------+
 *
 * UPDATE PROCESS:
 * 1. Download new firmware to inactive slot
 * 2. Verify downloaded firmware (CRC, signature)
 * 3. Mark inactive slot as pending
 * 4. Reboot
 * 5. Bootloader checks pending slot, boots it
 * 6. Application confirms successful boot
 * 7. Bootloader marks slot as confirmed
 *
 * ROLLBACK TRIGGERS:
 * - Boot count exceeds threshold (3 attempts)
 * - Application explicitly requests rollback
 * - Watchdog timeout during boot
 */

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>

// ============ SIMULATED FLASH MEMORY ============

const size_t BOOTLOADER_SIZE = 0x10000;      // 64KB
const size_t BOOT_CONFIG_SIZE = 0x1000;      // 4KB
const size_t FIRMWARE_SLOT_SIZE = 0x80000;   // 512KB per slot
const size_t PERSISTENT_SIZE = 0x10000;      // 64KB

const size_t BOOTLOADER_ADDR = 0x00000000;
const size_t BOOT_CONFIG_ADDR = 0x00010000;
const size_t SLOT_A_ADDR = 0x00020000;
const size_t SLOT_B_ADDR = 0x000A0000;
const size_t PERSISTENT_ADDR = 0x00120000;
const size_t FLASH_SIZE = 0x00130000;        // ~1.2MB total

static uint8_t g_flash[FLASH_SIZE];

void flash_init() {
    memset(g_flash, 0xFF, FLASH_SIZE);
}

void flash_read(uint32_t address, uint8_t* buffer, size_t length) {
    if (address + length <= FLASH_SIZE) {
        memcpy(buffer, &g_flash[address], length);
    }
}

bool flash_write(uint32_t address, const uint8_t* data, size_t length) {
    if (address + length > FLASH_SIZE) return false;
    for (size_t i = 0; i < length; i++) {
        g_flash[address + i] &= data[i];
    }
    return true;
}

bool flash_erase(uint32_t address, size_t length) {
    if (address + length > FLASH_SIZE) return false;
    memset(&g_flash[address], 0xFF, length);
    return true;
}

// ============ OTA SYSTEM STRUCTURES ============

// Boot slot identifiers
enum BootSlot : uint8_t {
    SLOT_A = 0,
    SLOT_B = 1,
    SLOT_NONE = 0xFF
};

// Slot state flags
enum SlotState : uint8_t {
    STATE_EMPTY = 0xFF,      // No firmware
    STATE_DOWNLOADING = 0xFE, // Download in progress
    STATE_READY = 0xFD,      // Download complete, verified
    STATE_PENDING = 0xFC,    // Marked for boot
    STATE_ACTIVE = 0xFB,     // Currently running
    STATE_CONFIRMED = 0xFA,  // Boot successful
    STATE_ROLLBACK = 0xF0    // Marked for rollback
};

// Boot configuration stored in flash
struct BootConfig {
    uint32_t magic;              // Config validity marker
    uint8_t active_slot;         // Currently active slot
    uint8_t pending_slot;        // Slot to try on next boot
    uint8_t boot_attempts;       // Failed boot counter
    uint8_t reserved;
    uint32_t slot_a_version;     // Version in slot A
    uint32_t slot_b_version;     // Version in slot B
    uint8_t slot_a_state;        // State of slot A
    uint8_t slot_b_state;        // State of slot B
    uint8_t padding[2];
    uint32_t crc;                // Config CRC
};

const uint32_t BOOT_CONFIG_MAGIC = 0x424F4F54;  // "BOOT"
const uint8_t MAX_BOOT_ATTEMPTS = 3;

// Firmware header (at start of each firmware image)
struct FirmwareHeader {
    uint32_t magic;              // Firmware validity marker
    uint32_t version;            // Firmware version
    uint32_t size;               // Total firmware size
    uint32_t crc;                // Firmware CRC
    uint32_t entry_point;        // Execution start address
    uint8_t signature[64];       // Cryptographic signature
    uint8_t reserved[32];
};

const uint32_t FIRMWARE_MAGIC = 0x464D5752;  // "FMWR"

// ============ YOUR IMPLEMENTATION ============

class OTAManager {
private:
    BootConfig config;
    bool config_loaded;

    // TODO: Add any additional state tracking

public:
    OTAManager() : config_loaded(false) {}

    /**
     * Initialize OTA manager, load boot config.
     */
    bool init() {
        // TODO: Implement
        // 1. Read boot config from flash
        // 2. Verify config CRC
        // 3. If invalid, create default config

        return false;  // Placeholder
    }

    /**
     * Start downloading new firmware to inactive slot.
     *
     * @return true if download can begin
     */
    bool begin_update() {
        // TODO: Implement
        // 1. Determine inactive slot
        // 2. Erase inactive slot
        // 3. Mark slot as DOWNLOADING
        // 4. Save config

        return false;  // Placeholder
    }

    /**
     * Write a chunk of firmware data.
     *
     * @param offset  Offset within firmware image
     * @param data    Data to write
     * @param length  Data length
     * @return true on success
     */
    bool write_chunk(uint32_t offset, const uint8_t* data, size_t length) {
        // TODO: Implement
        // 1. Validate offset and length
        // 2. Write to inactive slot at appropriate address
        // 3. Track progress

        return false;  // Placeholder
    }

    /**
     * Complete download and verify firmware.
     *
     * @return true if firmware is valid and ready
     */
    bool finalize_update() {
        // TODO: Implement
        // 1. Read firmware header
        // 2. Verify CRC
        // 3. Verify signature (optional)
        // 4. Mark slot as READY

        return false;  // Placeholder
    }

    /**
     * Activate the new firmware (will boot on next reset).
     *
     * @return true if activation successful
     */
    bool activate_update() {
        // TODO: Implement
        // 1. Verify slot is READY
        // 2. Mark slot as PENDING
        // 3. Reset boot attempts counter
        // 4. Save config

        return false;  // Placeholder
    }

    /**
     * Called by bootloader to determine which slot to boot.
     *
     * @return Slot to boot
     */
    BootSlot get_boot_slot() {
        // TODO: Implement
        // 1. If pending slot exists and attempts < max:
        //    - Increment attempts
        //    - Return pending slot
        // 2. Otherwise:
        //    - Clear pending
        //    - Return active slot (rollback)

        return SLOT_NONE;  // Placeholder
    }

    /**
     * Called by application after successful boot to confirm update.
     * Prevents rollback on subsequent boots.
     */
    bool confirm_boot() {
        // TODO: Implement
        // 1. Mark current slot as CONFIRMED
        // 2. Update active_slot to current slot
        // 3. Clear pending_slot
        // 4. Reset boot attempts
        // 5. Save config

        return false;  // Placeholder
    }

    /**
     * Manually trigger rollback to previous firmware.
     */
    bool request_rollback() {
        // TODO: Implement
        // 1. Mark current slot for rollback
        // 2. Set pending to other slot
        // 3. Save config

        return false;  // Placeholder
    }

    /**
     * Get current firmware version.
     */
    uint32_t get_current_version() {
        // TODO: Implement
        return 0;
    }

    /**
     * Get pending firmware version (if any).
     */
    uint32_t get_pending_version() {
        // TODO: Implement
        return 0;
    }

    /**
     * Check if update is in progress.
     */
    bool is_update_in_progress() {
        // TODO: Implement
        return false;
    }

    /**
     * Abort current update.
     */
    void abort_update() {
        // TODO: Implement
    }

private:
    /**
     * Get address of specified slot.
     */
    uint32_t get_slot_address(BootSlot slot) {
        return (slot == SLOT_A) ? SLOT_A_ADDR : SLOT_B_ADDR;
    }

    /**
     * Get inactive slot.
     */
    BootSlot get_inactive_slot() {
        return (config.active_slot == SLOT_A) ? SLOT_B : SLOT_A;
    }

    /**
     * Calculate CRC32.
     */
    uint32_t calculate_crc(const uint8_t* data, size_t length) {
        // Simple CRC implementation
        uint32_t crc = 0xFFFFFFFF;
        for (size_t i = 0; i < length; i++) {
            crc ^= data[i];
            for (int j = 0; j < 8; j++) {
                crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
            }
        }
        return ~crc;
    }

    /**
     * Save boot config to flash.
     */
    bool save_config() {
        config.crc = calculate_crc((uint8_t*)&config, offsetof(BootConfig, crc));
        flash_erase(BOOT_CONFIG_ADDR, BOOT_CONFIG_SIZE);
        return flash_write(BOOT_CONFIG_ADDR, (uint8_t*)&config, sizeof(config));
    }

    /**
     * Load boot config from flash.
     */
    bool load_config() {
        flash_read(BOOT_CONFIG_ADDR, (uint8_t*)&config, sizeof(config));

        if (config.magic != BOOT_CONFIG_MAGIC) return false;

        uint32_t expected_crc = calculate_crc((uint8_t*)&config,
                                               offsetof(BootConfig, crc));
        return config.crc == expected_crc;
    }
};

// ============ TEST FRAMEWORK ============

// Create a dummy firmware image for testing
void create_test_firmware(uint8_t* buffer, size_t size, uint32_t version) {
    memset(buffer, 0xFF, size);

    FirmwareHeader* header = (FirmwareHeader*)buffer;
    header->magic = FIRMWARE_MAGIC;
    header->version = version;
    header->size = size;
    header->entry_point = 0x00020100;

    // Fill with pattern
    for (size_t i = sizeof(FirmwareHeader); i < size; i++) {
        buffer[i] = (uint8_t)(i & 0xFF);
    }

    // Calculate CRC
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < size; i++) {
        if (i >= offsetof(FirmwareHeader, crc) && i < offsetof(FirmwareHeader, crc) + 4)
            continue;  // Skip CRC field
        crc ^= buffer[i];
    }
    header->crc = crc;
}

bool test_basic_ota() {
    printf("--- Basic OTA Tests ---\n");
    int passed = 0;
    int total = 0;

    flash_init();
    OTAManager ota;

    // Initialize
    if (ota.init()) {
        printf("PASS: OTA manager initialized\n");
        passed++;
    } else {
        printf("FAIL: OTA init failed\n");
    }
    total++;

    // Begin update
    if (ota.begin_update()) {
        printf("PASS: Update started\n");
        passed++;
    } else {
        printf("FAIL: begin_update failed\n");
    }
    total++;

    // Write firmware
    const size_t FW_SIZE = 1024;  // Small test firmware
    uint8_t firmware[FW_SIZE];
    create_test_firmware(firmware, FW_SIZE, 2);

    bool write_ok = true;
    const size_t CHUNK_SIZE = 256;
    for (size_t offset = 0; offset < FW_SIZE && write_ok; offset += CHUNK_SIZE) {
        size_t len = std::min(CHUNK_SIZE, FW_SIZE - offset);
        write_ok = ota.write_chunk(offset, &firmware[offset], len);
    }

    if (write_ok) {
        printf("PASS: Firmware written\n");
        passed++;
    } else {
        printf("FAIL: Firmware write failed\n");
    }
    total++;

    // Finalize
    if (ota.finalize_update()) {
        printf("PASS: Update finalized\n");
        passed++;
    } else {
        printf("FAIL: finalize_update failed\n");
    }
    total++;

    // Activate
    if (ota.activate_update()) {
        printf("PASS: Update activated\n");
        passed++;
    } else {
        printf("FAIL: activate_update failed\n");
    }
    total++;

    printf("Basic OTA tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_rollback() {
    printf("--- Rollback Tests ---\n");
    int passed = 0;
    int total = 0;

    flash_init();
    OTAManager ota;
    ota.init();

    // Simulate multiple failed boots (without calling confirm_boot)
    for (int i = 0; i < MAX_BOOT_ATTEMPTS + 1; i++) {
        BootSlot slot = ota.get_boot_slot();
        printf("Boot attempt %d: slot %d\n", i + 1, slot);

        if (i >= MAX_BOOT_ATTEMPTS) {
            // Should have rolled back
            if (slot == SLOT_A) {  // Assuming A was original
                printf("PASS: Rolled back after max attempts\n");
                passed++;
            } else {
                printf("FAIL: Did not rollback correctly\n");
            }
        }
    }
    total++;

    printf("Rollback tests: %d/%d passed\n\n", passed, total);
    return passed == total;
}

bool test_power_failure() {
    printf("--- Power Failure Tests ---\n");

    flash_init();
    OTAManager ota1;
    ota1.init();

    // Start update
    ota1.begin_update();

    // Write partial firmware (simulate power failure during download)
    uint8_t data[256];
    memset(data, 0xAA, sizeof(data));
    ota1.write_chunk(0, data, sizeof(data));

    // Simulate power failure - reinitialize
    OTAManager ota2;
    if (ota2.init()) {
        printf("PASS: Recovered from power failure during download\n");

        // Should be able to restart update
        if (ota2.begin_update()) {
            printf("PASS: Can restart update after power failure\n");
            return true;
        }
    }

    printf("FAIL: Power failure recovery\n");
    return false;
}

int main() {
    printf("Running Q19: OTA Update System\n");
    printf("==============================\n\n");

    bool all_passed = true;

    if (!test_basic_ota()) all_passed = false;
    if (!test_rollback()) all_passed = false;
    if (!test_power_failure()) all_passed = false;

    printf("==============================\n");
    if (all_passed) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed. Keep working on your solution.\n");
        return 1;
    }
}
