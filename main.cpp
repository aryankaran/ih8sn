#include <fstream>
#include <map>
#include <vector>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

void property_override(char const prop[], char const value[], bool add = false) {
    auto pi = (prop_info *) __system_property_find(prop);

    if (pi != nullptr) {
        __system_property_update(pi, value, strlen(value));
    } else if (add) {
        __system_property_add(prop, strlen(prop), value, strlen(value));
    }
}

void property_override(const std::vector<std::string> &props, char const value[], bool add = false) {
    for (const auto &prop : props) {
        property_override(prop.c_str(), value, add);
    }
}

std::vector<std::string> property_list(const std::string &prefix, const std::string &suffix) {
    std::vector<std::string> props;

    for (const std::string &part : {
        "",
        "bootimage.",
        "odm_dlkm.",
        "odm.",
        "oem.",
        "product.",
        "system_ext.",
        "system.",
        "vendor_dlkm.",
        "vendor.",
    }) {
        props.emplace_back(prefix + part + suffix);
    }

    return props;
}

int main(int argc, char *argv[]) {
    if (__system_properties_init()) {
        return -1;
    }

    if (argc != 2) {
        return -1;
    }

    const auto is_init_stage = strcmp(argv[1], "init") == 0;
    const auto is_boot_completed_stage = strcmp(argv[1], "boot_completed") == 0;

    if (is_init_stage) {
        property_override(property_list("ro.", "build.fingerprint"), "Xiaomi/dipper/dipper:8.1.0/OPM1.171019.011/V9.5.5.0.OEAMIFA:user/release-keys");
        property_override(property_list("ro.", "build.tags"), "release-keys");
        property_override(property_list("ro.", "build.type"), "user");
        property_override("ro.debuggable", "0");
    }

    if (is_boot_completed_stage) {
        property_override("ro.boot.flash.locked", "1");
        property_override("ro.boot.vbmeta.device_state", "locked");
        property_override("ro.boot.verifiedbootstate", "green");
        property_override("ro.boot.veritymode", "enforcing");
        property_override("ro.boot.warranty_bit", "0");
        property_override("ro.warranty_bit", "0");
    }

    return 0;
}
