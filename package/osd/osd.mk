################################################################################
#
# osd
#
################################################################################

OSD_VERSION = 1.0
OSD_SITE = $(BR2_EXTERNAL_SIRSIPE_EXTERNALS_PATH)/package/osd/src
OSD_SITE_METHOD = local
OSD_DEPENDENCIES = gpsd raylib

define OSD_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define OSD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/osd $(TARGET_DIR)/usr/bin
endef

define OSD_CONFIGURE_MODPROBE_I2C
        if ! grep -Fq "modprobe i2c-dev" $(TARGET_DIR)/etc/inittab; \
        then \
                $(SED) '/# now run any rc scripts/i\::sysinit:\/sbin\/modprobe i2c-dev' $(TARGET_DIR)/etc/inittab; \
        fi
	if ! grep -Fq "modprobe i2c-bcm2835" $(TARGET_DIR)/etc/inittab; \
        then \
                $(SED) '/# now run any rc scripts/i\::sysinit:\/sbin\/modprobe i2c-bcm2835' $(TARGET_DIR)/etc/inittab; \
        fi
endef

define OSD_CONFIGURE_GPSD
        $(SED) 's/\$$PIDFILE $$DEVICES/$$PIDFILE -n $$DEVICES/' $(TARGET_DIR)/etc/init.d/S50gpsd
endef

define OSD_CONFIGURE_CMDS
	$(OSD_CONFIGURE_MODPROBE_I2C)
        $(OSD_CONFIGURE_GPSD)
endef

$(eval $(generic-package))
