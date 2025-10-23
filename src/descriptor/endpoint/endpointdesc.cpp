#include "endpointdesc.h"

QT_USB_NAMESPACE_BEGIN

EndPointDesc::EndPointDesc(libusb_device *device, libusb_endpoint_descriptor desc)
    : DescriptorBase(device)
    , desc(desc)
{
    descriptorType = DescriptorType::ENDPOINT_DESCRIPTOR;
    printPrefix = QString(static_cast<int>(descriptorType), ' ');
}

void EndPointDesc::resolveInfo() {
    if(!children.isEmpty()) {
        return;
    }
    content += genContentLine(QString(24, '#'));;
    content += genContentLine("Point Address:", QString::number(desc.bEndpointAddress));
    content += genContentLine("Point Number:", QString::number(desc.bEndpointAddress & 0xF));
    content += genContentLine("Transfer Direction:", (desc.bEndpointAddress  & 0x80) == 0 ? "Host -> Dev" : "Host <- Dev");
    content += genContentLine("Transfer Type:", parseTransferType(desc.bmAttributes & 3));
    content += genContentLine("Max Packet Size:", QString::number(desc.wMaxPacketSize));
    content += genContentLine("Query Interval:", QString::number(desc.bInterval));
}

QString EndPointDesc::parseTransferType(int type) const {
    switch(type) {
        case 0: return "Control Transfer";
        case 1: return "Sync Transfer";
        case 2: return "Bulk Transfer";
        case 3: return "Interrupt Transfer";
    }
    return "Unknown Transfer Type";
}

QT_USB_NAMESPACE_END
