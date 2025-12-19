#include "endpointdesc.h"

QT_USB_NAMESPACE_BEGIN

EndPointDesc::EndPointDesc(libusb_device *device, libusb_endpoint_descriptor desc, InterfaceData* interfaceData)
    : DescriptorBase(device)
    , desc(desc)
    , interfaceData(interfaceData)
{
    descriptorType = DescriptorType::ENDPOINT_DESCRIPTOR;
    printPrefix = QString(static_cast<int>(descriptorType), ' ');
}

void EndPointDesc::resolveInfo() {
    if(!children.isEmpty()) {
        return;
    }
    auto transferType = parseTransferType(desc.bmAttributes & 3);
    content += genContentLine(QString(24, '#'));;
    content += genContentLine("Point Address:", QString::number(desc.bEndpointAddress));
    content += genContentLine("Point Number:", QString::number(desc.bEndpointAddress & 0xF), " [Point Number is used when setConfiguration]");
    content += genContentLine("Transfer Direction:", (desc.bEndpointAddress  & 0x80) == 0 ? "Host -> Dev" : "Host <- Dev");
    content += genContentLine("Transfer Type:", transferTypeToString(transferType));
    content += genContentLine("Max Packet Size:", QString::number(desc.wMaxPacketSize));
    content += genContentLine("Query Interval:", QString::number(desc.bInterval));
    EndPointData endPointData;
    endPointData.address = desc.bEndpointAddress;
    endPointData.transferType = transferType;
    endPointData.direction = (desc.bEndpointAddress  & 0x80) == 0 ? TransferDirection::HOST_TO_DEVICE : TransferDirection::DEVICE_TO_HOST;
    endPointData.maxPacketSize = desc.wMaxPacketSize;
    interfaceData->endpoints.insert(desc.bEndpointAddress, endPointData);
}

TransferType EndPointDesc::parseTransferType(int type) const {
    Q_ASSERT(type >= 0 && type < 4);
    return static_cast<TransferType>(type);
}

QT_USB_NAMESPACE_END
