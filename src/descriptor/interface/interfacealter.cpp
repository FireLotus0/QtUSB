#include "interfacealter.h"
#include  "src/descriptor/endpoint/endpointdesc.h"

QT_USB_NAMESPACE_BEGIN

InterfaceAlter::InterfaceAlter(libusb_device *device, libusb_interface_descriptor interfaceDesc)
    : DescriptorBase(device)
    , desc(interfaceDesc)
{
    descriptorType = DescriptorType::INTERFACE_ALTERNATIVE;
    printPrefix = QString(static_cast<int>(descriptorType), ' ');
}

void InterfaceAlter::resolveInfo() {
    if(!children.isEmpty()) {
        return;
    }
    content += genContentLine("Interface Number:", QString::number(desc.bInterfaceNumber));
    content += genContentLine("Alternate Setting:", QString::number(desc.bAlternateSetting));
    content += genContentLine("EndPoint Counts:", QString::number(desc.bNumEndpoints));
    for(int i = 0; i < desc.bNumEndpoints; i++) {
        auto endPoint = new EndPointDesc(device, desc.endpoint[i]);
        endPoint->resolveInfo();
        children.append(endPoint);
    }
}

QT_USB_NAMESPACE_END