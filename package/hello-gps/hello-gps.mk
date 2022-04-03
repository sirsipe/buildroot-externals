################################################################################
#
# hello-gps
#
################################################################################

HELLO_GPS_VERSION = 1.0
HELLO_GPS_SITE = $(BR2_EXTERNAL_SIRSIPE_EXTERNALS_PATH)/package/hello-gps/src
HELLO_GPS_SITE_METHOD = local
HELLP_GPS_INSTALL_STAGING = yes
HELLO_GPS_DEPENDENCIES = gpsd

define HELLO_GPS_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define HELLO_GPS_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/hello-gps $(TARGET_DIR)/usr/bin
	$(SED) 's/$PIDFILE $DEVICES/$PIDFILE -n $DEVICES/' $(TARGET_DIR)/etc/init.d/S50gpsd
endef

$(eval $(generic-package))
