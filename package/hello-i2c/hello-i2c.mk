################################################################################
#
# hello-i2c
#
################################################################################

HELLO_I2C_VERSION = 1.0
HELLO_I2C_SITE = $(BR2_EXTERNAL_SIRSIPE_EXTERNALS_PATH)/package/hello-i2c/src
HELLO_I2C_SITE_METHOD = local
#HELLO_I2C_DEPENDENCIES = m

define HELLO_I2C_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) CC="$(TARGET_CC)" -C $(@D) hello-i2c
endef

define HELLO_I2C_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/hello-i2c $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
