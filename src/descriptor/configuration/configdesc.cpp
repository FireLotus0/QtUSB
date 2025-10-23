#include "configdesc.h"
#include "src/descriptor/interface/interfacedesc.h"
QT_USB_NAMESPACE_BEGIN

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
        qWarning() << "Get config descriptor failed: config index:" << configIndex;
    } else {
        content += genContentLine("Configuration #", QString::number(configIndex), " ID=", QString::number(desc->bConfigurationValue));
        content += genContentLine("Interface Counts: ", QString::number(desc->bNumInterfaces));
        for(int i = 0; i < desc->bNumInterfaces; i++) {
            ConfigurationData configurationData;
            configurationData.configurationValue = desc->bConfigurationValue;
            descriptorData->configurations.insert(desc->bConfigurationValue, configurationData);
            auto interface = new InterfaceDesc(device, desc->interface[i], i, &configurationData);
            interface->resolveInfo();
            children.append(interface);
        }
    }
    libusb_free_config_descriptor(desc);
}


QT_USB_NAMESPACE_END
