#include "stdafx.h"
#include "ArenaApi.h"
#include "SaveApi.h"
#include  <algorithm>// for std::find
#include <thread>	 // for sleep
#include <chrono>

#define TAB1 "  "
#define TAB2 "    "
#define TAB3 "      "
#define ERASE_LINE "                            "

#define EXPOSURE_TIME 2000.0
#define PTPSYNC_FRAME_RATE 20
#define HEIGHT 2048
#define BINTYPE "Sum"
#define BINNUM 1

// =-=-=-=-=-=-=-=-=-
// =-=- SETTINGS =-=-
// =-=-=-=-=-=-=-=-=-

// Image timeout
//    Timeout for grabbing images (in milliseconds). If no image is available at
//    the end of the timeout, an exception is thrown. The timeout is the maximum
//    time to wait for an image; however, getting an image will return as soon as
//    an image is available, not waiting the full extent of the timeout.
#define TIMEOUT 200000

// number of images to grab
#define NUM_IMAGES 30000

#define PACKET_DELAY 0
#define TRANS_DELAY 8000
#define PIXEL_FORMAT "BayerRG8"


// =-=-=-=-=-=-=-=-=-
// =-=- EXAMPLE -=-=-
// =-=-=-=-=-=-=-=-=-

// 

void SaveImage(Arena::IImage* pImage, const char* filename)
{
	// Arena::EBayerAlgorithm bayerAlgorithm = DirectionalInterpolation;
	// auto pConverted = Arena::ImageFactory::Convert(
	// 	pImage,
	// 	RGB8, 
	// 	Arena::DirectionalInterpolation);

	// prepare image parameters
	std::cout << TAB1 << "Prepare image parameters\n";
	std::cout << TAB1 << "Width: " << pImage->GetWidth() << std::endl;
	std::cout << TAB1 << "GetHeight: " << pImage->GetHeight() << std::endl;
	std::cout << TAB1 << "GetBitsPerPixel: " << pImage->GetBitsPerPixel() << std::endl;

	Save::ImageParams params(
		pImage->GetWidth(),
		pImage->GetHeight(),
		pImage->GetBitsPerPixel());

	// prepare image writer
	std::cout << TAB1 << "Prepare image writer\n";

	Save::ImageWriter writer(
		params,
		filename);

	// Set image writer to BMP
	//   Set the output file format of the image writer to BMP.
	//   The writer saves the image file as BMP file even without
	//	 the extension in the file name. Aside from this setting,
	//   compression can be set several different compression algorithms, 
	//   and store tags for separated CMYK by changing the parameters.
	std::cout << TAB1 << "Set image writer to png\n";

	writer.SetBmp(".bmp");
			
	// save image
	std::cout << TAB1 << "Save image\n";

	writer << pImage->GetData();
	// Arena::ImageFactory::Destroy(pConverted);
}

void PTPSyncCamerasAndAcquireImages(Arena::ISystem* pSystem, std::vector<Arena::IDevice*>& devices)
{

	for (size_t i = 0; i < devices.size(); i++)
	{
		Arena::IDevice* pDevice = devices.at(i);
		GenICam::gcstring deviceSerialNumber = Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "DeviceSerialNumber");

		std::cout << TAB2 << "Prepare camera " << deviceSerialNumber << "\n";

		// Manually set exposure time
		//    In order to get synchronized images, the exposure time must be
		//    synchronized as well.
		std::cout << TAB3 << "Exposure: ";

		Arena::SetNodeValue<GenICam::gcstring>(
			pDevice->GetNodeMap(),
			"ExposureAuto",
			"Off");

		// std::cout << TAB3 << "Hello";

		Arena::SetNodeValue<double>(
			pDevice->GetNodeMap(),
			"ExposureTime",
			EXPOSURE_TIME);

		std::cout << Arena::GetNodeValue<double>(pDevice->GetNodeMap(), "ExposureTime") << "\n";

		// Synchronize devices by enabling PTP
		//    Enabling PTP on multiple devices causes them to negotiate amongst
		//    themselves so that there is a single master device while all the
		//    rest become slaves. The slaves' clocks all synchronize to the
		//    master's clock.
		std::cout << TAB3 << "PTP: ";

		Arena::SetNodeValue<bool>(
			pDevice->GetNodeMap(),
			"PtpEnable",
			true);

		std::cout << (Arena::GetNodeValue<bool>(pDevice->GetNodeMap(), "PtpEnable") ? "enabled" : "disabled") << "\n";

		// Use max supported packet size. We use transfer control to ensure that only one camera
		// is transmitting at a time.
		std::cout << TAB3 << "StreamAutoNegotiatePacketSize: ";
		Arena::SetNodeValue<bool>(pDevice->GetTLStreamNodeMap(), "StreamAutoNegotiatePacketSize", true);
		std::cout << Arena::GetNodeValue<bool>(pDevice->GetTLStreamNodeMap(), "StreamAutoNegotiatePacketSize") << "\n";

		// enable stream packet resend
		std::cout << TAB3 << "StreamPacketResendEnable: ";
		Arena::SetNodeValue<bool>(pDevice->GetTLStreamNodeMap(), "StreamPacketResendEnable", true);
		std::cout << Arena::GetNodeValue<bool>(pDevice->GetTLStreamNodeMap(), "StreamPacketResendEnable") << "\n";

		// Set acquisition mode to 'Continuous'
		std::cout << TAB3 << "Set acquisition mode to 'Continuous'\n";
		Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionMode", "Continuous");

		//Set acquisition start mode to 'PTPSync'
		std::cout << TAB3 << "Set acquisition start mode to 'PTPSync'\n";
        // std::cout << "actual acq astart mode: " << Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionStartMode") << "\n";
		Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionStartMode", "PTPSync");
		
		// 	Set StreamBufferHandlingMode to 'NewestOnly'
		std::cout << TAB3 << "Set StreamBufferHandlingMode to 'NewestOnly'\n";
		Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetTLStreamNodeMap(), "StreamBufferHandlingMode", "NewestOnly");
		
		// Set pixel format to Mono8
		Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "PixelFormat", PIXEL_FORMAT);
		std::cout << TAB3 << "Set pixel format to: " << Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "PixelFormat") << std::endl;
		
		if (i == 0)
		{
			// Packet Delay 
			GenApi::CIntegerPtr pStreamChannelPacketDelay = pDevice->GetNodeMap()->GetNode("GevSCPD");
			pStreamChannelPacketDelay->SetValue(PACKET_DELAY);
			std::cout << TAB3 << "GevSCPD: ";
			std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCPD") << "\n";

			// Transmission Delay 
			GenApi::CIntegerPtr pStreamChannelFrameTransmissionDelay = pDevice->GetNodeMap()->GetNode("GevSCFTD");
			pStreamChannelFrameTransmissionDelay->SetValue(0);
			std::cout << TAB3 << "GevSCFTD: ";
			std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCFTD") << "\n";
		}

		else if (i == 1)
		{
			// Packet Delay 
			GenApi::CIntegerPtr pStreamChannelPacketDelay = pDevice->GetNodeMap()->GetNode("GevSCPD");
			pStreamChannelPacketDelay->SetValue(PACKET_DELAY);
			std::cout << TAB3 << "GevSCPD: ";
			std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCPD") << "\n";

			// Transmission Delay 
			GenApi::CIntegerPtr pStreamChannelFrameTransmissionDelay = pDevice->GetNodeMap()->GetNode("GevSCFTD");
			pStreamChannelFrameTransmissionDelay->SetValue(TRANS_DELAY);
			std::cout << TAB3 << "GevSCFTD: ";
			std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCFTD") << "\n";
		}

		else if (i == 2)
		{
			// Packet Delay 
			GenApi::CIntegerPtr pStreamChannelPacketDelay = pDevice->GetNodeMap()->GetNode("GevSCPD");
			pStreamChannelPacketDelay->SetValue(PACKET_DELAY);
			std::cout << TAB3 << "GevSCPD: ";
			std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCPD") << "\n";

			// Transmission Delay 
			GenApi::CIntegerPtr pStreamChannelFrameTransmissionDelay = pDevice->GetNodeMap()->GetNode("GevSCFTD");
			pStreamChannelFrameTransmissionDelay->SetValue(2*TRANS_DELAY);
			std::cout << TAB3 << "GevSCFTD: ";
			std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCFTD") << "\n";
		}

		else if (i == 3)
		{
			// Packet Delay 
			GenApi::CIntegerPtr pStreamChannelPacketDelay = pDevice->GetNodeMap()->GetNode("GevSCPD");
			pStreamChannelPacketDelay->SetValue(PACKET_DELAY);
			std::cout << TAB3 << "GevSCPD: ";
			std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCPD") << "\n";

			// Transmission Delay 
			GenApi::CIntegerPtr pStreamChannelFrameTransmissionDelay = pDevice->GetNodeMap()->GetNode("GevSCFTD");
			pStreamChannelFrameTransmissionDelay->SetValue(3*TRANS_DELAY);
			std::cout << TAB3 << "GevSCFTD: ";
			std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCFTD") << "\n";
		}

		// Image Height
		
		// std::cout << "hi" << std::endl;
		// gain
		GenApi::CFloatPtr gain = pDevice->GetNodeMap()->GetNode("Gain");
		// std::cout << "woah" << std::endl;
		gain->SetValue(13);
		// std::cout << "wow!" << std::endl;

		// Binning
		GenApi::CIntegerPtr vbin = pDevice->GetNodeMap()->GetNode("BinningVertical");
		GenApi::CIntegerPtr hbin = pDevice->GetNodeMap()->GetNode("BinningHorizontal");
		vbin->SetValue(BINNUM);
		hbin->SetValue(BINNUM);

		Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "BinningVerticalMode", BINTYPE);
		Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "BinningHorizontalMode", BINTYPE);
		
		GenApi::CIntegerPtr height = pDevice->GetNodeMap()->GetNode("Height");
		height->SetValue(HEIGHT / BINNUM);
		// Frame rate
		GenApi::CFloatPtr pAcquisitionFrameRate = pDevice->GetNodeMap()->GetNode("AcquisitionFrameRate");
		pAcquisitionFrameRate->SetValue(pAcquisitionFrameRate->GetMax());
        std::cout << "Acq Frame Rate: " << pAcquisitionFrameRate->GetMax() << std::endl;

		// PTPSyncFrameRate
		GenApi::CFloatPtr pPTPSyncFrameRate = pDevice->GetNodeMap()->GetNode("PTPSyncFrameRate");
		pPTPSyncFrameRate->SetValue(PTPSYNC_FRAME_RATE);

		std::cout << "stream channel packet size: " << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "DeviceStreamChannelPacketSize") << std::endl;

		
		
	}

	// prepare system
	std::cout << TAB2 << "Prepare system\n";
	
	// Wait for devices to negotiate their PTP relationship
	//    Before starting any PTP-dependent actions, it is important to wait for
	//    the devices to complete their negotiation; otherwise, the devices may
	//    not yet be synced. Depending on the initial PTP state of each camera,
	//    it can take about 40 seconds for all devices to autonegotiate. Below,
	//    we wait for the PTP status of each device until there is only one
	//    'Master' and the rest are all 'Slaves'. During the negotiation phase,
	//    multiple devices may initially come up as Master so we will wait until
	//    the ptp negotiation completes.
	std::cout << TAB1 << "Wait for devices to negotiate. This can take up to about 40s.\n";

	// std::vector serials;
	int i = 0;
	do
	{
		bool masterFound = false;
		bool restartSyncCheck = false;

		// check devices
		for (size_t j = 0; j < devices.size(); j++)
		{
			Arena::IDevice* pDevice = devices.at(j);

			// get PTP status
			GenICam::gcstring ptpStatus = Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "PtpStatus");
            std::cout << ptpStatus<< std::endl;

			if (ptpStatus == "Master")
			{
				if (masterFound)
				{
					// Multiple masters -- ptp negotiation is not complete
					restartSyncCheck = true;
					break;
				}

				masterFound = true;
			}
			else if (ptpStatus != "Slave")
			{
				// Uncalibrated state -- ptp negotiation is not complete
				restartSyncCheck = true;
				break;
			}
		}

		// A single master was found and all remaining cameras are slaves
		if (!restartSyncCheck && masterFound)
			break;

		std::this_thread::sleep_for(std::chrono::duration<int>(1));

		// for output
		if (i % 10 == 0)
			std::cout << "\r" << ERASE_LINE << "\r" << TAB2 << std::flush;

		std::cout << "." << std::flush;

		i++;

	} while (true);


	// start stream
	std::cout << "\n"
		<< TAB1 << "Start stream\n";

	for (size_t i = 0; i < devices.size(); i++)
	{
		devices.at(i)->StartStream();
	}


	// get images and check timestamps
	std::cout << TAB1 << "Get images\n";
	auto start = std::chrono::high_resolution_clock::now();

	for (size_t i = 0; i < NUM_IMAGES; i++)
	{
		for (size_t j = 0; j < devices.size(); j++)
		{
			Arena::IDevice* pDevice = devices.at(j);
			GenICam::gcstring deviceSerialNumber = Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "DeviceSerialNumber");
            Arena::IImage* pImage = pDevice->GetImage(20000);
			std::cout << TAB2 << "Image " << i << " from device " << deviceSerialNumber << "\n";

			// Compare timestamps
			//    Scheduling action commands amongst PTP synchronized devices results
			//    in synchronized images with synchronized timestamps.
			
            std::cout << TAB3 << "Timestamp: ";

			
            // std::cout << "hi" << std::endl;
            // Arena::IBuffer* pBuffer = pDevice->GetBuffer(20000);
            // std::cout << TAB3 << "buffersize: " << pBuffer->HasImageData()<< std::endl;
			std::cout << pImage->GetTimestamp() << "\n";
            // char* filename = pImage->GetTimestamp();
			std::string camnum = deviceSerialNumber.c_str();
			
			std::string fname = "/media/external/" + camnum + "/" + std::to_string(i);
			std::cout << "saving to: " << fname << std::endl;
            SaveImage(pImage, fname.c_str());

			// requeue buffer
			pDevice->RequeueBuffer(pImage);
		}
	}
	auto stop = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);


	std::cout << "time to capture: " <<  duration.count()/1000000  << std::endl;
	std::cout << "average frame rate: " << 1000000 * NUM_IMAGES/(duration.count())  << std::endl;
	
	
	// stop stream
	std::cout << TAB1 << "Stop stream\n";

	for (size_t i = 0; i < devices.size(); i++)
	{
		devices.at(i)->StopStream();
	}
	
	
}

// =-=-=-=-=-=-=-=-=-
// =- PREPARATION -=-
// =- & CLEAN UP =-=-
// =-=-=-=-=-=-=-=-=-

int main()
{
	// flag to track when an exception has been thrown
	bool exceptionThrown = false;

	std::cout << "Cpp_PTPSync\n";

	try
	{
		// prepare example
		std::cout << "Preparing\n";
		Arena::ISystem* pSystem = Arena::OpenSystem();
		pSystem->UpdateDevices(100);
		std::vector<Arena::DeviceInfo> deviceInfos = pSystem->GetDevices();
		std::cout << "Got Devices: " << deviceInfos.size()<< "\n";
		for (size_t i = 0; i < deviceInfos.size(); i++){
			std::cout << "Got Devices: " << deviceInfos[i].IpAddressStr()<< "\n";	
			if (deviceInfos[i].IpAddressStr() == "169.254.1.1"){
				std::cout << "Removing bad device\n";
				deviceInfos.pop_back();
			}
		}
		if (deviceInfos.size() < 2)
		{
			if (deviceInfos.size() == 0)
				std::cout << "\nNo camera connected. Example requires at least 2 devices\n";
			else if (deviceInfos.size() == 1)
				std::cout << "\nOnly one device connected. Example requires at least 2 devices\n";

			std::cout << "Press enter to complete\n";

			// clear input
			while (std::cin.get() != '\n')
				continue;

			std::getchar();
			return 0;
		}
		std::vector<Arena::IDevice*> devices;
		for (size_t i = 0; i < deviceInfos.size(); i++)
		{
			devices.push_back(pSystem->CreateDevice(deviceInfos.at(i)));
		}

		// run example
		std::cout << "Commence example\n\n";
		PTPSyncCamerasAndAcquireImages(pSystem, devices);
		std::cout << "\nExample complete\n";

		// clean up example
		for (size_t i = 0; i < devices.size(); i++)
		{
			pSystem->DestroyDevice(devices.at(i));
		}
		Arena::CloseSystem(pSystem);
	}
	catch (GenICam::GenericException& ge)
	{
		std::cout << "\nGenICam exception thrown: " << ge.what() << "\n";
		exceptionThrown = true;
	}
	catch (std::exception& ex)
	{
		std::cout << "\nStandard exception thrown: " << ex.what() << "\n";
		exceptionThrown = true;
	}
	catch (...)
	{
		std::cout << "\nUnexpected exception thrown\n";
		exceptionThrown = true;
	}

	std::cout << "Press enter to complete\n";
	std::getchar();

	if (exceptionThrown)
		return -1;
	else
		return 0;
}