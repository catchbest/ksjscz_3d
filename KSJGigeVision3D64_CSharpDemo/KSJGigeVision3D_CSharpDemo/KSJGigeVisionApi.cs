using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace KSJGigeVisionApi_Space
{
    class KSJGigeVisionApi
    {
        
        //public struct ADAPTER_INFO
        //{
        //    string         szFriendlyName;
        //    string         szDescription;
        //    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
        //    byte[] ucDns;
        //    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)]
        //    byte[] ucIp;
        //    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        //    byte[] ucMac;
        //};


		[DllImport("MD_KSJGigeVisionApi_x64.dll")]
        public extern static int KSJGIGEVISION_Init(int nType = 0, string lpszServerIP = null);
		
        [DllImport("MD_KSJGigeVisionApi_x64.dll")]
        public extern static int KSJGIGEVISION_UnInit();

        [DllImport("MD_KSJGigeVisionApi_x64.dll")]
        public extern static int KSJGIGEVISION_AdapterGetCount();

        //[DllImport("MD_KSJGigeVisionApi_x64.dll")]
        //public extern static int KSJGIGEVISION_AdapterGetInformation(int nAdapterIndex, ref ADAPTER_INFO pInfo);

        [DllImport("MD_KSJGigeVisionApi_x64.dll")]
        public extern static int KSJGIGEVISION_CameraGetCount(int nAdapterIndex);

        //public struct CAMERA_INFO
        //{
        //    public ushort SpecVersionMajor;
        //    public ushort SpecVersionMinor;
        //    public uint DeviceMode;
        //    public ushort DeviceMacAddressH;
        //    public uint DeviceMacAddressL;
        //    public uint IpConfigOptions;
        //    public uint IpConfigCurrent;
        //    public uint CurrentIp;
        //    public uint CurrentSubnetMask;
        //    public uint DefaultGateway;
        //    public string ManufacturerName;
        //    public string ModelName;
        //    public string DeviceVersion;
        //    public string ManufacturerSpecificInformation;
        //    public string SerialNumber;
        //    public string UserDefinedName;
        //};

        //[DllImport("MD_KSJGigeVisionApi_x64.dll")]
        //public extern static int KSJGIGEVISION_CameraGetInformation(int nAdapterIndex, int nCameraIndex, ref CAMERA_INFO pCameraInfo);

        [DllImport("MD_KSJGigeVisionApi_x64.dll")]
        public extern static int KSJGIGEVISION_CameraForceIp(int nAdapterIndex, int nCameraIndex, string szIP);

        //public struct GVCP_ACK_HEADER
        //{
        //    public ushort Status;          // GEV_STATUS_SUCCESS
        //    public ushort Answer; 	        // GVCP_xxxx_ACK
        //    public ushort Length;		    // GVCP响应结构的GVCP_ACK_HEADER结构后面的数据字节数
        //    public ushort AckId;
        //};

        //public struct GIGE_DISCOVERY_ACK
        //{
        //    public GVCP_ACK_HEADER AckHeader;
        //    public ushort SpecVersionMajor;
        //    public ushort SpecVersionMinor;
        //    public uint DeviceMode;
        //    public ushort Reserved;
        //    public ushort DeviceMacAddressH;
        //    public uint DeviceMacAddressL;
        //    public uint IpConfigOptions;
        //    public uint IpConfigCurrent;
        //    public uint Reserved0;
        //    public uint Reserved1;
        //    public uint Reserved2;
        //    public uint CurrentIp;
        //    public uint Reserved3;
        //    public uint Reserved4;
        //    public uint Reserved5;
        //    public uint CurrentSubnetMask;
        //    public uint Reserved6;
        //    public uint Reserved7;
        //    public uint Reserved8;
        //    public uint DefaultGateway;
        //    public string ManufacturerName;
        //    public string ModelName;
        //    public string DeviceVersion;
        //    public string ManufacturerSpecificInformation;
        //    public string SerialNumber;
        //    public string UserDefinedName;
        //};


        //[DllImport("MD_KSJGigeVisionApi_x64.dll")]
        //public extern static int KSJGIGEVISION_CameraRefresh(int nAdapterIndex, int nCameraIndex, ref GIGE_DISCOVERY_ACK DiscoveryAck);

        [DllImport("MD_KSJGigeVisionApi_x64.dll")]
        public extern static int KSJGIGEVISION_ReadRegister(int nAdapterIndex, int nCameraIndex, uint RegAddress, ref uint pRegData);

        [DllImport("MD_KSJGigeVisionApi_x64.dll")]
        public extern static int KSJGIGEVISION_WriteRegister(int nAdapterIndex, int nCameraIndex, uint RegAddress, uint RegData);

        [DllImport("MD_KSJGigeVisionApi_x64.dll")]
        public extern static int KSJGIGEVISION_CameraSetGvspPort(int nAdapterIndex, int nCameraIndex, int nGvspPort);

        [DllImport("MD_KSJGigeVisionApi_x64.dll")]
        public extern static int KSJGIGEVISION_CameraSetGvspSize(int nAdapterIndex, int nCameraIndex, int nGvspSize);

        [DllImport("MD_KSJGigeVisionApi_x64.dll")]
        public extern static int KSJGIGEVISION_CameraGetGvspData(int nAdapterIndex, int nCameraIndex, byte[] pGvspData);

        [DllImport("MD_KSJGigeVisionApi_x64.dll")]
        public extern static int KSJGIGEVISION_CameraGet3DGvspData(int nAdapterIndex, int nCameraIndex, int nType, byte[] pGvspData, ref int pnWidth, ref int pnHeight);
    }
}
