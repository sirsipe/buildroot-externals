################################################################################
#
# hello-raylib
#
################################################################################

HELLO_RAYLIB_VERSION = 1.0
HELLO_RAYLIB_SITE = $(BR2_EXTERNAL_SIRSIPE_EXTERNALS_PATH)/package/hello-raylib/src
HELLO_RAYLIB_SITE_METHOD = local
HELLO_RAYLIB_DEPENDENCIES = raylib

define HELLO_RAYLIB_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) CC="$(TARGET_CC)" -C $(@D) hello-raylib
endef

define HELLO_RAYLIB_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/hello-raylib $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
