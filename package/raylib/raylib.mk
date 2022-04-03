################################################################################
#
# raylib
#
################################################################################

RAYLIB_VERSION = 4.0.0
RAYLIB_API_VERSION = 400
RAYLIB_SITE = $(call github,raysan5,raylib,$(RAYLIB_VERSION))
RAYLIB_DEPENDENCIES = libgles
RAYLIB_INSTALL_STAGING = YES

define RAYLIB_BUILD_CMDS
	$(MAKE) PLATFORM=PLATFORM_RPI RAYLIB_LIBTYPE=SHARED CC="$(TARGET_CC) -D_POSIX_C_SOURCE=199309L" -C $(@D)/src raylib
endef

define RAYLIB_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0644 $(@D)/src/libraylib.so.$(RAYLIB_VERSION) $(TARGET_DIR)/usr/lib/libraylib.so.$(RAYLIB_VERSION)
	ln -fsvr $(TARGET_DIR)/usr/lib/libraylib.so.$(RAYLIB_VERSION) $(TARGET_DIR)/usr/lib/libraylib.so.$(RAYLIB_API_VERSION)
	ln -fsvr $(TARGET_DIR)/usr/lib/libraylib.so.$(RAYLIB_API_VERSION) $(TARGET_DIR)/usr/lib/libraylib.so
endef

define RAYLIB_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0644 $(@D)/src/raylib.h $(STAGING_DIR)/usr/include/raylib.h
        $(INSTALL) -D -m 0644 $(@D)/src/libraylib.so.$(RAYLIB_VERSION) $(STAGING_DIR)/usr/lib/libraylib.so.$(RAYLIB_VERSION)
	ln -fsvr $(STAGING_DIR)/usr/lib/libraylib.so.$(RAYLIB_VERSION) $(STAGING_DIR)/usr/lib/libraylib.so.$(RAYLIB_API_VERSION)
        ln -fsvr $(STAGING_DIR)/usr/lib/libraylib.so.$(RAYLIB_API_VERSION) $(STAGING_DIR)/usr/lib/libraylib.so
endef

$(eval $(generic-package))
