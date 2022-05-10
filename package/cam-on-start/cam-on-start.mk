################################################################################
#
# cam-on-start
#
################################################################################

CAM_ON_START_VERSION = 1.0
CAM_ON_START_SITE = $(BR2_EXTERNAL_SIRSIPE_EXTERNALS_PATH)/package/cam-on-start/src
CAM_ON_START_SITE_METHOD = local

define CAM_ON_START_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/S00Cam $(TARGET_DIR)/etc/init.d/S00Cam
	$(INSTALL) -D -m 0755 $(@D)/start-raspivid.sh $(TARGET_DIR)/usr/bin/start-raspivid.sh
endef

$(eval $(generic-package))
