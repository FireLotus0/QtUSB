#include "interfacedesc.h"
#include "interfacealter.h"

QT_USB_NAMESPACE_BEGIN


InterfaceDesc::InterfaceDesc(libusb_device *device, libusb_interface interface, int index, ConfigurationData* configurationData)
    : DescriptorBase(device)
    , interface(interface)
    , interfaceIndex(index)
    , configurationData(configurationData)
{
    descriptorType = DescriptorType::INTERFACE_DESCRIPTOR;
    printPrefix = QString(static_cast<int>(descriptorType), ' ');
}

void InterfaceDesc::resolveInfo() {
    if(!children.isEmpty()) {
        return;
    }
    content += genContentLine("Interface #", QString::number(interfaceIndex));
    content += genContentLine("Alternative Counts:", QString::number(interface.num_altsetting));
    for(int i = 0; i < interface.num_altsetting; i++) {
        auto alterInterface = new InterfaceAlter(device, interface.altsetting[i], configurationData);
        alterInterface->resolveInfo();
        children.append(alterInterface);
    }
}

QT_USB_NAMESPACE_END
