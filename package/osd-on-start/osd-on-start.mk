################################################################################
#
# osd-on-start
#
################################################################################

OSD_ON_START_VERSION = 1.0
OSD_ON_START_SITE = $(BR2_EXTERNAL_SIRSIPE_EXTERNALS_PATH)/package/osd-on-start/src
OSD_ON_START_SITE_METHOD = local

define OSD_ON_START_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/S99OSD $(TARGET_DIR)/etc/init.d/S99OSD
	$(INSTALL) -D -m 0755 $(@D)/start-osd.sh $(TARGET_DIR)/usr/bin/start-osd.sh
endef

$(eval $(generic-package))
