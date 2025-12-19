#include "configdesc.h"
#include "descriptor/interface/interfacedesc.h"

#include <qloggingcategory.h>

QT_USB_NAMESPACE_BEGIN

const QLoggingCategory &usbCategory();

ConfigDesc::ConfigDesc(libusb_device *device, int index, DescriptorData* descriptorData)
    : DescriptorBase(device)
    , configIndex(index)
    , descriptorData(descriptorData)
{
    descriptorType = DescriptorType::CONFIGURATION_DESCRIPTOR;
    printPrefix = QString(static_cast<int>(descriptorType), ' ');
}


void ConfigDesc::resolveInfo() {
    if(!children.isEmpty()) {
        return;
    }
    auto rc = libusb_get_config_descriptor(device, configIndex, &desc);
    if(rc != LIBUSB_SUCCESS) {
        qCWarning(usbCategory) << "Get config descriptor failed: config index:" << configIndex;
    } else {
        content += genContentLine("Configuration #", QString::number(configIndex), " ID=", QString::number(desc->bConfigurationValue), " [ID is used when setConfiguration]");
        content += genContentLine("Interface Counts: ", QString::number(desc->bNumInterfaces));
        ConfigurationData configurationData;
        configurationData.configurationValue = desc->bConfigurationValue;
        descriptorData->configurations.insert(desc->bConfigurationValue, configurationData);
        for(int i = 0; i < desc->bNumInterfaces; i++) {
            auto interface = new InterfaceDesc(device, desc->interface[i], i, &descriptorData->configurations[desc->bConfigurationValue]);
            interface->resolveInfo();
            children.append(interface);
        }
    }
    libusb_free_config_descriptor(desc);
}


QT_USB_NAMESPACE_END
