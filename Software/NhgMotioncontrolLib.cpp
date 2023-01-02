#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <utility>
#include <limits.h>
#include <stdio.h>

#include "NhgMotionControlLib.h"

#ifndef _NTDEF_
typedef LONG NTSTATUS;
#endif

#ifdef __MINGW32__
#include <ntdef.h>
#include <winbase.h>
#endif

#ifdef __CYGWIN__
#include <ntdef.h>
#define _wcsdup wcsdup
#endif

/* The maximum number of characters that can be passed into the
   HidD_Get*String() functions without it failing.*/
#define MAX_STRING_WCHARS 0xFFF
   /*#define HIDAPI_USE_DDK*/

extern "C" {

#include <setupapi.h>
#include <winioctl.h>
#ifdef HIDAPI_USE_DDK
#include <hidsdi.h>
#endif

	/* Copied from inc/ddk/hidclass.h, part of the Windows DDK. */
#define HID_OUT_CTL_CODE(id)  \
		CTL_CODE(FILE_DEVICE_KEYBOARD, (id), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_HID_GET_FEATURE                   HID_OUT_CTL_CODE(100)


} /* extern "C" */

#ifndef HIDAPI_H__
#define HIDAPI_H__
#include <wchar.h>

extern "C" {
	struct hid_device_;
	typedef struct hid_device_ hid_device; /**< opaque hidapi structure */

	/** hidapi info structure */
	struct hid_device_info {
		/** Platform-specific device path */
		char* path;
		/** Device Vendor ID */
		unsigned short vendor_id;
		/** Device Product ID */
		unsigned short product_id;
		/** Serial Number */
		wchar_t* serial_number;
		/** Device Release Number in binary-coded decimal,
			also known as Device Version Number */
		unsigned short release_number;
		/** Manufacturer String */
		wchar_t* manufacturer_string;
		/** Product string */
		wchar_t* product_string;
		/** Usage Page for this Device/Interface
			(Windows/Mac only). */
		unsigned short usage_page;
		/** Usage for this Device/Interface
			(Windows/Mac only).*/
		unsigned short usage;
		/** The USB interface which this logical device
			represents. Valid on both Linux implementations
			in all cases, and valid on the Windows implementation
			only if the device contains more than one interface. */
		int interface_number;

		/** Pointer to the next device */
		struct hid_device_info* next;
	};

	int hid_init(void);
	int hid_exit(void);
	struct hid_device_info* hid_enumerate(unsigned short vendor_id, unsigned short product_id);
	void hid_free_enumeration(struct hid_device_info* devs);
	hid_device* hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t* serial_number);
	hid_device* hid_open_path(const char* path);
	int hid_write(hid_device* device, const unsigned char* data, size_t length);
	int hid_read_timeout(hid_device* dev, unsigned char* data, size_t length, int milliseconds);
	int hid_read(hid_device* device, unsigned char* data, size_t length);
	int hid_set_nonblocking(hid_device* device, int nonblock);
	int hid_send_feature_report(hid_device* device, const unsigned char* data, size_t length);
	int hid_get_feature_report(hid_device* device, unsigned char* data, size_t length);
	void hid_close(hid_device* device);
	int hid_get_manufacturer_string(hid_device* device, wchar_t* string, size_t maxlen);
	int hid_get_product_string(hid_device* device, wchar_t* string, size_t maxlen);
	int hid_get_serial_number_string(hid_device* device, wchar_t* string, size_t maxlen);
	int hid_get_indexed_string(hid_device* device, int string_index, wchar_t* string, size_t maxlen);
	const wchar_t* hid_error(hid_device* device);
}
#endif

#undef MIN
#define MIN(x,y) ((x) < (y)? (x): (y))

#ifdef _MSC_VER
/* Thanks Microsoft, but I know how to use strncpy(). */
#pragma warning(disable:4996)
#endif

extern "C" {


#ifndef HIDAPI_USE_DDK
	/* Since we're not building with the DDK, and the HID header
	   files aren't part of the SDK, we have to define all this
	   stuff here. In lookup_functions(), the function pointers
	   defined below are set. */
	typedef struct _HIDD_ATTRIBUTES {
		ULONG Size;
		USHORT VendorID;
		USHORT ProductID;
		USHORT VersionNumber;
	} HIDD_ATTRIBUTES, * PHIDD_ATTRIBUTES;

	typedef USHORT USAGE;
	typedef struct _HIDP_CAPS {
		USAGE Usage;
		USAGE UsagePage;
		USHORT InputReportByteLength;
		USHORT OutputReportByteLength;
		USHORT FeatureReportByteLength;
		USHORT Reserved[17];
		USHORT fields_not_used_by_hidapi[10];
	} HIDP_CAPS, * PHIDP_CAPS;
	typedef void* PHIDP_PREPARSED_DATA;
#define HIDP_STATUS_SUCCESS 0x110000

	typedef BOOLEAN(__stdcall* HidD_GetAttributes_)(HANDLE device, PHIDD_ATTRIBUTES attrib);
	typedef BOOLEAN(__stdcall* HidD_GetSerialNumberString_)(HANDLE device, PVOID buffer, ULONG buffer_len);
	typedef BOOLEAN(__stdcall* HidD_GetManufacturerString_)(HANDLE handle, PVOID buffer, ULONG buffer_len);
	typedef BOOLEAN(__stdcall* HidD_GetProductString_)(HANDLE handle, PVOID buffer, ULONG buffer_len);
	typedef BOOLEAN(__stdcall* HidD_SetFeature_)(HANDLE handle, PVOID data, ULONG length);
	typedef BOOLEAN(__stdcall* HidD_GetFeature_)(HANDLE handle, PVOID data, ULONG length);
	typedef BOOLEAN(__stdcall* HidD_GetIndexedString_)(HANDLE handle, ULONG string_index, PVOID buffer, ULONG buffer_len);
	typedef BOOLEAN(__stdcall* HidD_GetPreparsedData_)(HANDLE handle, PHIDP_PREPARSED_DATA* preparsed_data);
	typedef BOOLEAN(__stdcall* HidD_FreePreparsedData_)(PHIDP_PREPARSED_DATA preparsed_data);
	typedef NTSTATUS(__stdcall* HidP_GetCaps_)(PHIDP_PREPARSED_DATA preparsed_data, HIDP_CAPS* caps);
	typedef BOOLEAN(__stdcall* HidD_SetNumInputBuffers_)(HANDLE handle, ULONG number_buffers);

	static HidD_GetAttributes_ HidD_GetAttributes;
	static HidD_GetSerialNumberString_ HidD_GetSerialNumberString;
	static HidD_GetManufacturerString_ HidD_GetManufacturerString;
	static HidD_GetProductString_ HidD_GetProductString;
	static HidD_SetFeature_ HidD_SetFeature;
	static HidD_GetFeature_ HidD_GetFeature;
	static HidD_GetIndexedString_ HidD_GetIndexedString;
	static HidD_GetPreparsedData_ HidD_GetPreparsedData;
	static HidD_FreePreparsedData_ HidD_FreePreparsedData;
	static HidP_GetCaps_ HidP_GetCaps;
	static HidD_SetNumInputBuffers_ HidD_SetNumInputBuffers;

	static HMODULE lib_handle = NULL;
	static BOOLEAN initialized = FALSE;
#endif /* HIDAPI_USE_DDK */

	struct hid_device_ {
		HANDLE device_handle;
		BOOL blocking;
		USHORT output_report_length;
		size_t input_report_length;
		void* last_error_str;
		DWORD last_error_num;
		BOOL read_pending;
		char* read_buf;
		OVERLAPPED ol;
	};

	static hid_device* new_hid_device()
	{
		hid_device* dev = (hid_device*)calloc(1, sizeof(hid_device));
		dev->device_handle = INVALID_HANDLE_VALUE;
		dev->blocking = TRUE;
		dev->output_report_length = 0;
		dev->input_report_length = 0;
		dev->last_error_str = NULL;
		dev->last_error_num = 0;
		dev->read_pending = FALSE;
		dev->read_buf = NULL;
		memset(&dev->ol, 0, sizeof(dev->ol));
		dev->ol.hEvent = CreateEvent(NULL, FALSE, FALSE /*initial state f=nonsignaled*/, NULL);

		return dev;
	}

	static void free_hid_device(hid_device* dev)
	{
		CloseHandle(dev->ol.hEvent);
		CloseHandle(dev->device_handle);
		LocalFree(dev->last_error_str);
		free(dev->read_buf);
		free(dev);
	}

	static void register_error(hid_device* device, const char* op)
	{
		WCHAR* ptr, * msg;

		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&msg, 0/*sz*/,
			NULL);

		/* Get rid of the CR and LF that FormatMessage() sticks at the
		   end of the message. Thanks Microsoft! */
		ptr = msg;
		while (*ptr) {
			if (*ptr == '\r') {
				*ptr = 0x0000;
				break;
			}
			ptr++;
		}

		/* Store the message off in the Device entry so that
		   the hid_error() function can pick it up. */
		LocalFree(device->last_error_str);
		device->last_error_str = msg;
	}

#ifndef HIDAPI_USE_DDK
	static int lookup_functions()
	{
		lib_handle = LoadLibraryA("hid.dll");
		if (lib_handle) {
#define RESOLVE(x) x = (x##_)GetProcAddress(lib_handle, #x); if (!x) return -1;
			RESOLVE(HidD_GetAttributes);
			RESOLVE(HidD_GetSerialNumberString);
			RESOLVE(HidD_GetManufacturerString);
			RESOLVE(HidD_GetProductString);
			RESOLVE(HidD_SetFeature);
			RESOLVE(HidD_GetFeature);
			RESOLVE(HidD_GetIndexedString);
			RESOLVE(HidD_GetPreparsedData);
			RESOLVE(HidD_FreePreparsedData);
			RESOLVE(HidP_GetCaps);
			RESOLVE(HidD_SetNumInputBuffers);
#undef RESOLVE
		}
		else
			return -1;

		return 0;
	}
#endif

	static HANDLE open_device(const char* path, BOOL enumerate)
	{
		HANDLE handle;
		DWORD desired_access = (enumerate) ? 0 : (GENERIC_WRITE | GENERIC_READ);
		DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

		handle = CreateFileA(path,
			desired_access,
			share_mode,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,/*FILE_ATTRIBUTE_NORMAL,*/
			0);

		return handle;
	}

	int hid_init(void)
	{
#ifndef HIDAPI_USE_DDK
		if (!initialized) {
			if (lookup_functions() < 0) {
				hid_exit();
				return -1;
			}
			initialized = TRUE;
		}
#endif
		return 0;
	}

	int   hid_exit(void)
	{
#ifndef HIDAPI_USE_DDK
		if (lib_handle)
			FreeLibrary(lib_handle);
		lib_handle = NULL;
		initialized = FALSE;
#endif
		return 0;
	}

	struct hid_device_info* hid_enumerate(unsigned short vendor_id, unsigned short product_id)
	{
		BOOL res;
		struct hid_device_info* root = NULL; /* return object */
		struct hid_device_info* cur_dev = NULL;

		/* Windows objects for interacting with the driver. */
		GUID InterfaceClassGuid = { 0x4d1e55b2, 0xf16f, 0x11cf, {0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30} };
		SP_DEVINFO_DATA devinfo_data;
		SP_DEVICE_INTERFACE_DATA device_interface_data;
		SP_DEVICE_INTERFACE_DETAIL_DATA_A* device_interface_detail_data = NULL;
		HDEVINFO device_info_set = INVALID_HANDLE_VALUE;
		int device_index = 0;
		int i;

		if (hid_init() < 0)
			return NULL;

		/* Initialize the Windows objects. */
		memset(&devinfo_data, 0x0, sizeof(devinfo_data));
		devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
		device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		/* Get information for all the devices belonging to the HID class. */
		device_info_set = SetupDiGetClassDevsA(&InterfaceClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

		/* Iterate over each device in the HID class, looking for the right one. */

		for (;;) {
			HANDLE write_handle = INVALID_HANDLE_VALUE;
			DWORD required_size = 0;
			HIDD_ATTRIBUTES attrib;

			res = SetupDiEnumDeviceInterfaces(device_info_set,
				NULL,
				&InterfaceClassGuid,
				device_index,
				&device_interface_data);

			if (!res) {
				/* A return of FALSE from this function means that
				   there are no more devices. */
				break;
			}

			/* Call with 0-sized detail size, and let the function
			   tell us how long the detail struct needs to be. The
			   size is put in &required_size. */
			res = SetupDiGetDeviceInterfaceDetailA(device_info_set,
				&device_interface_data,
				NULL,
				0,
				&required_size,
				NULL);

			/* Allocate a long enough structure for device_interface_detail_data. */
			device_interface_detail_data = (SP_DEVICE_INTERFACE_DETAIL_DATA_A*)malloc(required_size);
			device_interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_A);

			/* Get the detailed data for this device. The detail data gives us
			   the device path for this device, which is then passed into
			   CreateFile() to get a handle to the device. */
			res = SetupDiGetDeviceInterfaceDetailA(device_info_set,
				&device_interface_data,
				device_interface_detail_data,
				required_size,
				NULL,
				NULL);

			if (!res) {
				/* register_error(dev, "Unable to call SetupDiGetDeviceInterfaceDetail");
				   Continue to the next device. */
				goto cont;
			}

			/* Make sure this device is of Setup Class "HIDClass" and has a
			   driver bound to it. */
			for (i = 0; ; i++) {
				char driver_name[256];

				/* Populate devinfo_data. This function will return failure
				   when there are no more interfaces left. */
				res = SetupDiEnumDeviceInfo(device_info_set, i, &devinfo_data);
				if (!res)
					goto cont;

				res = SetupDiGetDeviceRegistryPropertyA(device_info_set, &devinfo_data,
					SPDRP_CLASS, NULL, (PBYTE)driver_name, sizeof(driver_name), NULL);
				if (!res)
					goto cont;

				if (strcmp(driver_name, "HIDClass") == 0) {
					/* See if there's a driver bound. */
					res = SetupDiGetDeviceRegistryPropertyA(device_info_set, &devinfo_data,
						SPDRP_DRIVER, NULL, (PBYTE)driver_name, sizeof(driver_name), NULL);
					if (res)
						break;
				}
			}

			//wprintf(L"HandleName: %s\n", device_interface_detail_data->DevicePath);

			/* Open a handle to the device */
			write_handle = open_device(device_interface_detail_data->DevicePath, TRUE);

			/* Check validity of write_handle. */
			if (write_handle == INVALID_HANDLE_VALUE) {
				/* Unable to open the device. */
				//register_error(dev, "CreateFile");
				goto cont_close;
			}


			/* Get the Vendor ID and Product ID for this device. */
			attrib.Size = sizeof(HIDD_ATTRIBUTES);
			HidD_GetAttributes(write_handle, &attrib);
			//wprintf(L"Product/Vendor: %x %x\n", attrib.ProductID, attrib.VendorID);

			/* Check the VID/PID to see if we should add this
			   device to the enumeration list. */
			if ((vendor_id == 0x0 || attrib.VendorID == vendor_id) &&
				(product_id == 0x0 || attrib.ProductID == product_id)) {

#define WSTR_LEN 512
				const char* str;
				struct hid_device_info* tmp;
				PHIDP_PREPARSED_DATA pp_data = NULL;
				HIDP_CAPS caps;
				BOOLEAN res;
				NTSTATUS nt_res;
				wchar_t wstr[WSTR_LEN]; /* TODO: Determine Size */
				size_t len;

				/* VID/PID match. Create the record. */
				tmp = (struct hid_device_info*)calloc(1, sizeof(struct hid_device_info));
				if (cur_dev) {
					cur_dev->next = tmp;
				}
				else {
					root = tmp;
				}
				cur_dev = tmp;

				/* Get the Usage Page and Usage for this device. */
				res = HidD_GetPreparsedData(write_handle, &pp_data);
				if (res) {
					nt_res = HidP_GetCaps(pp_data, &caps);
					if (nt_res == HIDP_STATUS_SUCCESS) {
						cur_dev->usage_page = caps.UsagePage;
						cur_dev->usage = caps.Usage;
					}

					HidD_FreePreparsedData(pp_data);
				}

				/* Fill out the record */
				cur_dev->next = NULL;
				str = device_interface_detail_data->DevicePath;
				if (str) {
					len = strlen(str);
					cur_dev->path = (char*)calloc(len + 1, sizeof(char));
					strncpy(cur_dev->path, str, len + 1);
					cur_dev->path[len] = '\0';
				}
				else
					cur_dev->path = NULL;

				/* Serial Number */
				res = HidD_GetSerialNumberString(write_handle, wstr, sizeof(wstr));
				wstr[WSTR_LEN - 1] = 0x0000;
				if (res) {
					cur_dev->serial_number = _wcsdup(wstr);
				}

				/* Manufacturer String */
				res = HidD_GetManufacturerString(write_handle, wstr, sizeof(wstr));
				wstr[WSTR_LEN - 1] = 0x0000;
				if (res) {
					cur_dev->manufacturer_string = _wcsdup(wstr);
				}

				/* Product String */
				res = HidD_GetProductString(write_handle, wstr, sizeof(wstr));
				wstr[WSTR_LEN - 1] = 0x0000;
				if (res) {
					cur_dev->product_string = _wcsdup(wstr);
				}

				/* VID/PID */
				cur_dev->vendor_id = attrib.VendorID;
				cur_dev->product_id = attrib.ProductID;

				/* Release Number */
				cur_dev->release_number = attrib.VersionNumber;

				/* Interface Number. It can sometimes be parsed out of the path
				   on Windows if a device has multiple interfaces. See
				   http://msdn.microsoft.com/en-us/windows/hardware/gg487473 or
				   search for "Hardware IDs for HID Devices" at MSDN. If it's not
				   in the path, it's set to -1. */
				cur_dev->interface_number = -1;
				if (cur_dev->path) {
					char* interface_component = strstr(cur_dev->path, "&mi_");
					if (interface_component) {
						char* hex_str = interface_component + 4;
						char* endptr = NULL;
						cur_dev->interface_number = strtol(hex_str, &endptr, 16);
						if (endptr == hex_str) {
							/* The parsing failed. Set interface_number to -1. */
							cur_dev->interface_number = -1;
						}
					}
				}
			}

		cont_close:
			CloseHandle(write_handle);
		cont:
			/* We no longer need the detail data. It can be freed */
			free(device_interface_detail_data);

			device_index++;

		}

		/* Close the device information handle. */
		SetupDiDestroyDeviceInfoList(device_info_set);

		return root;

	}

	void      hid_free_enumeration(struct hid_device_info* devs)
	{
		/* TODO: Merge this with the Linux version. This function is platform-independent. */
		struct hid_device_info* d = devs;
		while (d) {
			struct hid_device_info* next = d->next;
			free(d->path);
			free(d->serial_number);
			free(d->manufacturer_string);
			free(d->product_string);
			free(d);
			d = next;
		}
	}


	hid_device* hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t* serial_number)
	{
		/* TODO: Merge this functions with the Linux version. This function should be platform independent. */
		struct hid_device_info* devs, * cur_dev;
		const char* path_to_open = NULL;
		hid_device* handle = NULL;

		devs = hid_enumerate(vendor_id, product_id);
		cur_dev = devs;
		while (cur_dev) {
			if (cur_dev->vendor_id == vendor_id &&
				cur_dev->product_id == product_id) {
				if (serial_number) {
					if (wcscmp(serial_number, cur_dev->serial_number) == 0) {
						path_to_open = cur_dev->path;
						break;
					}
				}
				else {
					path_to_open = cur_dev->path;
					break;
				}
			}
			cur_dev = cur_dev->next;
		}

		if (path_to_open) {
			/* Open the device */
			handle = hid_open_path(path_to_open);
		}

		hid_free_enumeration(devs);

		return handle;
	}

	hid_device* hid_open_path(const char* path)
	{
		hid_device* dev;
		HIDP_CAPS caps;
		PHIDP_PREPARSED_DATA pp_data = NULL;
		BOOLEAN res;
		NTSTATUS nt_res;

		if (hid_init() < 0) {
			return NULL;
		}

		dev = new_hid_device();

		/* Open a handle to the device */
		dev->device_handle = open_device(path, FALSE);

		/* Check validity of write_handle. */
		if (dev->device_handle == INVALID_HANDLE_VALUE) {
			/* Unable to open the device. */
			register_error(dev, "CreateFile");
			goto err;
		}

		/* Set the Input Report buffer size to 64 reports. */
		res = HidD_SetNumInputBuffers(dev->device_handle, 64);
		if (!res) {
			register_error(dev, "HidD_SetNumInputBuffers");
			goto err;
		}

		/* Get the Input Report length for the device. */
		res = HidD_GetPreparsedData(dev->device_handle, &pp_data);
		if (!res) {
			register_error(dev, "HidD_GetPreparsedData");
			goto err;
		}
		nt_res = HidP_GetCaps(pp_data, &caps);
		if (nt_res != HIDP_STATUS_SUCCESS) {
			register_error(dev, "HidP_GetCaps");
			goto err_pp_data;
		}
		dev->output_report_length = caps.OutputReportByteLength;
		dev->input_report_length = caps.InputReportByteLength;
		HidD_FreePreparsedData(pp_data);

		dev->read_buf = (char*)malloc(dev->input_report_length);

		return dev;

	err_pp_data:
		HidD_FreePreparsedData(pp_data);
	err:
		free_hid_device(dev);
		return NULL;
	}

	int     hid_write(hid_device* dev, const unsigned char* data, size_t length)
	{
		DWORD bytes_written;
		BOOL res;

		OVERLAPPED ol;
		unsigned char* buf;
		memset(&ol, 0, sizeof(ol));

		/* Make sure the right number of bytes are passed to WriteFile. Windows
		   expects the number of bytes which are in the _longest_ report (plus
		   one for the report number) bytes even if the data is a report
		   which is shorter than that. Windows gives us this value in
		   caps.OutputReportByteLength. If a user passes in fewer bytes than this,
		   create a temporary buffer which is the proper size. */
		if (length >= dev->output_report_length) {
			/* The user passed the right number of bytes. Use the buffer as-is. */
			buf = (unsigned char*)data;
		}
		else {
			/* Create a temporary buffer and copy the user's data
			   into it, padding the rest with zeros. */
			buf = (unsigned char*)malloc(dev->output_report_length);
			memcpy(buf, data, length);
			memset(buf + length, 0, dev->output_report_length - length);
			length = dev->output_report_length;
		}

		res = WriteFile(dev->device_handle, buf, length, NULL, &ol);

		if (!res) {
			if (GetLastError() != ERROR_IO_PENDING) {
				/* WriteFile() failed. Return error. */
				register_error(dev, "WriteFile");
				bytes_written = -1;
				goto end_of_function;
			}
		}

		/* Wait here until the write is done. This makes
		   hid_write() synchronous. */
		res = GetOverlappedResult(dev->device_handle, &ol, &bytes_written, TRUE/*wait*/);
		if (!res) {
			/* The Write operation failed. */
			register_error(dev, "WriteFile");
			bytes_written = -1;
			goto end_of_function;
		}

	end_of_function:
		if (buf != data)
			free(buf);

		return bytes_written;
	}


	int     hid_read_timeout(hid_device* dev, unsigned char* data, size_t length, int milliseconds)
	{
		DWORD bytes_read = 0;
		size_t copy_len = 0;
		BOOL res;

		/* Copy the handle for convenience. */
		HANDLE ev = dev->ol.hEvent;

		if (!dev->read_pending) {
			/* Start an Overlapped I/O read. */
			dev->read_pending = TRUE;
			memset(dev->read_buf, 0, dev->input_report_length);
			ResetEvent(ev);
			res = ReadFile(dev->device_handle, dev->read_buf, dev->input_report_length, &bytes_read, &dev->ol);

			if (!res) {
				if (GetLastError() != ERROR_IO_PENDING) {
					/* ReadFile() has failed.
					   Clean up and return error. */
					CancelIo(dev->device_handle);
					dev->read_pending = FALSE;
					goto end_of_function;
				}
			}
		}

		if (milliseconds >= 0) {
			/* See if there is any data yet. */
			res = WaitForSingleObject(ev, milliseconds);
			if (res != WAIT_OBJECT_0) {
				/* There was no data this time. Return zero bytes available,
				   but leave the Overlapped I/O running. */
				return 0;
			}
		}

		/* Either WaitForSingleObject() told us that ReadFile has completed, or
		   we are in non-blocking mode. Get the number of bytes read. The actual
		   data has been copied to the data[] array which was passed to ReadFile(). */
		res = GetOverlappedResult(dev->device_handle, &dev->ol, &bytes_read, TRUE/*wait*/);

		/* Set pending back to false, even if GetOverlappedResult() returned error. */
		dev->read_pending = FALSE;

		if (res && bytes_read > 0) {
			if (dev->read_buf[0] == 0x0) {
				/* If report numbers aren't being used, but Windows sticks a report
				   number (0x0) on the beginning of the report anyway. To make this
				   work like the other platforms, and to make it work more like the
				   HID spec, we'll skip over this byte. */
				bytes_read--;
				copy_len = length > bytes_read ? bytes_read : length;
				memcpy(data, dev->read_buf + 1, copy_len);
			}
			else {
				/* Copy the whole buffer, report number and all. */
				copy_len = length > bytes_read ? bytes_read : length;
				memcpy(data, dev->read_buf, copy_len);
			}
		}

	end_of_function:
		if (!res) {
			register_error(dev, "GetOverlappedResult");
			return -1;
		}

		return copy_len;
	}

	int     hid_read(hid_device* dev, unsigned char* data, size_t length)
	{
		return hid_read_timeout(dev, data, length, (dev->blocking) ? -1 : 0);
	}

	int     hid_set_nonblocking(hid_device* dev, int nonblock)
	{
		dev->blocking = !nonblock;
		return 0; /* Success */
	}

	int     hid_send_feature_report(hid_device* dev, const unsigned char* data, size_t length)
	{
		BOOL res = HidD_SetFeature(dev->device_handle, (PVOID)data, length);
		if (!res) {
			register_error(dev, "HidD_SetFeature");
			return -1;
		}

		return length;
	}


	int     hid_get_feature_report(hid_device* dev, unsigned char* data, size_t length)
	{
		BOOL res;
#if 0
		res = HidD_GetFeature(dev->device_handle, data, length);
		if (!res) {
			register_error(dev, "HidD_GetFeature");
			return -1;
		}
		return 0; /* HidD_GetFeature() doesn't give us an actual length, unfortunately */
#else
		DWORD bytes_returned;

		OVERLAPPED ol;
		memset(&ol, 0, sizeof(ol));

		res = DeviceIoControl(dev->device_handle,
			IOCTL_HID_GET_FEATURE,
			data, length,
			data, length,
			&bytes_returned, &ol);

		if (!res) {
			if (GetLastError() != ERROR_IO_PENDING) {
				/* DeviceIoControl() failed. Return error. */
				register_error(dev, "Send Feature Report DeviceIoControl");
				return -1;
			}
		}

		/* Wait here until the write is done. This makes
		   hid_get_feature_report() synchronous. */
		res = GetOverlappedResult(dev->device_handle, &ol, &bytes_returned, TRUE/*wait*/);
		if (!res) {
			/* The operation failed. */
			register_error(dev, "Send Feature Report GetOverLappedResult");
			return -1;
		}

		/* bytes_returned does not include the first byte which contains the
		   report ID. The data buffer actually contains one more byte than
		   bytes_returned. */
		bytes_returned++;

		return bytes_returned;
#endif
	}

	void     hid_close(hid_device* dev)
	{
		if (!dev)
			return;
		CancelIo(dev->device_handle);
		free_hid_device(dev);
	}

	int      hid_get_manufacturer_string(hid_device* dev, wchar_t* string, size_t maxlen)
	{
		BOOL res;

		res = HidD_GetManufacturerString(dev->device_handle, string, sizeof(wchar_t) * MIN(maxlen, MAX_STRING_WCHARS));
		if (!res) {
			register_error(dev, "HidD_GetManufacturerString");
			return -1;
		}

		return 0;
	}

	int      hid_get_product_string(hid_device* dev, wchar_t* string, size_t maxlen)
	{
		BOOL res;

		res = HidD_GetProductString(dev->device_handle, string, sizeof(wchar_t) * MIN(maxlen, MAX_STRING_WCHARS));
		if (!res) {
			register_error(dev, "HidD_GetProductString");
			return -1;
		}

		return 0;
	}

	int      hid_get_serial_number_string(hid_device* dev, wchar_t* string, size_t maxlen)
	{
		BOOL res;

		res = HidD_GetSerialNumberString(dev->device_handle, string, sizeof(wchar_t) * MIN(maxlen, MAX_STRING_WCHARS));
		if (!res) {
			register_error(dev, "HidD_GetSerialNumberString");
			return -1;
		}

		return 0;
	}

	int      hid_get_indexed_string(hid_device* dev, int string_index, wchar_t* string, size_t maxlen)
	{
		BOOL res;

		res = HidD_GetIndexedString(dev->device_handle, string_index, string, sizeof(wchar_t) * MIN(maxlen, MAX_STRING_WCHARS));
		if (!res) {
			register_error(dev, "HidD_GetIndexedString");
			return -1;
		}

		return 0;
	}


	const wchar_t* hid_error(hid_device* dev)
	{
		return (wchar_t*)dev->last_error_str;
	}


	/*#define PICPGM*/
	/*#define S11*/
#define P32
#ifdef S11 
	unsigned short VendorID = 0xa0a0;
	unsigned short ProductID = 0x0001;
#endif

#ifdef P32
	unsigned short VendorID = 0x04d8;
	unsigned short ProductID = 0x3f;
#endif


#ifdef PICPGM
	unsigned short VendorID = 0x04d8;
	unsigned short ProductID = 0x0033;
#endif


#if 0
	int __cdecl main(int argc, char* argv[])
	{
		int res;
		unsigned char buf[65];

		UNREFERENCED_PARAMETER(argc);
		UNREFERENCED_PARAMETER(argv);

		/* Set up the command buffer. */
		memset(buf, 0x00, sizeof(buf));
		buf[0] = 0;
		buf[1] = 0x81;


		/* Open the device. */
		int handle = open(VendorID, ProductID, L"12345");
		if (handle < 0)
			printf("unable to open device\n");


		/* Toggle LED (cmd 0x80) */
		buf[1] = 0x80;
		res = write(handle, buf, 65);
		if (res < 0)
			printf("Unable to write()\n");

		/* Request state (cmd 0x81) */
		buf[1] = 0x81;
		write(handle, buf, 65);
		if (res < 0)
			printf("Unable to write() (2)\n");

		/* Read requested state */
		read(handle, buf, 65);
		if (res < 0)
			printf("Unable to read()\n");

		/* Print out the returned buffer. */
		for (int i = 0; i < 4; i++)
			printf("buf[%d]: %d\n", i, buf[i]);

		return 0;
	}
#endif
}

static int res;
static unsigned char buf[256];
#define MAX_STR 255
static wchar_t wstr[MAX_STR];
static hid_device* handle;
static int i;

struct hid_device_info* devs, * cur_dev;

enum task {
	MainMod = 1,
	DrvRel,        //relative drive
	DrvCRel,       //counter relative drive
	DrvPCont,      //+ continuous pulse drive
	DrvMCont,      //- continuous pulse drive
	DrvAbs,        // Absolute drive
	DecStp,        // Decelerating stop
	InsStp,        // Instant stops
	IoSt,          //Io seting
	IoCtr,         //Io general output control
	LedCtr,        //On board LED control
	GetPos,        //Get Logical Position and Real Position
	GetDrvPara,    //Get Acc, Dec, InitVel, Pos
	SetLpRp,       //Set value logical position and real position for feedback
	SetLp,         //Set value logical position
	SetRp,         //Set value real position
	ChangeVel,     //Override velocity
	VelDrive,      //Velocity drive specialty
	JogDrive,      //Jog drive
	UsbDrvWait,     //Return 1 if Under Driving
	SetAxistask,
	Test_func1_var
};


INT32 Div32(INT32 input32, UINT8* output8) {
	output8[0] = 0xFF & (input32 >> 24);
	output8[1] = 0xFF & (input32 >> 16);
	output8[2] = 0xFF & (input32 >> 8);
	output8[3] = 0xFF & (input32 >> 0);

	return 0;
}

#define ModOffset 5
#define IoCtrOffset 19
#define DrvOffset 19;



INT32 NhgIsOpen(wchar_t* Manufacturer, wchar_t* Product, wchar_t* SerialNumber) {
	handle = hid_open(0x461, 0x20, L"12110");
	if (!handle)
		return -2;
	hid_set_nonblocking(handle, 1);
	res = hid_read(handle, buf, 17);
	if (res != 0)
		return res; //non block 0, block -1.

	wchar_t KeyManufacturer[MAX_STR] = L"NHG";
	wchar_t KeyProduct[MAX_STR] = L"SINGLE AXIS MOTION CONTROLLER";
	wchar_t KeySerialNumber[MAX_STR] = L"12110";

	Manufacturer[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, Manufacturer, MAX_STR);
	//printf("Manufacturer:  %ls\n", Manufacturer);
	if (wcscmp(Manufacturer, KeyManufacturer) != 0)
		return -3; //Manufacturer not match

	Product[0] = 0x0000;
	res = hid_get_product_string(handle, Product, MAX_STR);
	if (wcscmp(Product, KeyProduct) != 0)
		return -4;

	SerialNumber[0] = 0x0000;
	res = hid_get_serial_number_string(handle, SerialNumber, MAX_STR);
	if (wcscmp(SerialNumber, KeySerialNumber) != 0)
		return -5; //SerialNumber not match
	return 0;
}


INT32 Test_func1(){
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = Test_func1_var;


	UsbTx[10] = 0xFF ;

	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write
	return 0;
}

INT32 SetAxis(INT32 axis) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = SetAxistask;
	

	UsbTx[10] = 0xFF & axis;
	
	res = hid_write(handle, UsbTx, 64);
	if (res < 0) return -5; //can't write
	return 0;
}

INT32 NhgMode(INT16 Mode1, INT16 Mode2, INT16 Mode3) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = MainMod;
	//Reg1
	if (Mode1 != 0) {
		UsbTx[5] = 0xFF & (Mode1 >> 8);
		UsbTx[6] = 0xFF & (Mode1);
	}
	//Reg2
	if (Mode2 != 0) {
		UsbTx[7] = 0xFF & (Mode2 >> 8);
		UsbTx[8] = 0xFF & (Mode2);
	}

	//Reg3
	if (Mode3 != 0) {
		UsbTx[9] = 0xFF & (Mode3 >> 8);
		UsbTx[10] = 0xFF & (Mode3);
	}
	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write
	return 0;
}

INT32 NhgIoSetup(INT16 Mode1, INT16 Mode2) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = IoSt;

	if (Mode1 != 0) {
		UsbTx[4] = 0x41;
		UsbTx[19] = 0xFF & (Mode1 >> 8);
		UsbTx[20] = 0xFF & (Mode1);
	}
	if (Mode2 != 0) {
		UsbTx[4] = 0x42;
		UsbTx[21] = 0xFF & (Mode2 >> 8);
		UsbTx[22] = 0xFF & (Mode2);
	}
	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write
	return 0;
}

INT32 NhgIoGOutCtr(UINT16 IoState) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;
	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = IoCtr;

	UsbTx[11] = 0x00;
	UsbTx[12] = (0xFF) & IoState;
	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write
	return 0;
}

INT32 NhgDrvRel(INT32 InitSpeed, INT32 Speed, INT32 Acc, INT32 Jerk, INT32 Pos, INT32 Dec) {
	if (InitSpeed < 0 || Speed < 0 || Acc < 0 || Jerk < 0 || Dec < 0)
		return -6; //some input can't < 0

	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = DrvRel;

	//UsbTx[2] to UsbTx[17] for reg0 to reg7
	//cmd Reg0 = 0x50 relative drive
	UsbTx[3] = 0x00;
	UsbTx[4] = 0x50;
	/*Direction signal */
	if (Pos < 0)
	{
		UsbTx[5] = 0x59;
	}
	else UsbTx[5] = 0x58;
	//InitSpeed
	int offset = 19;
	Div32(InitSpeed, &UsbTx[offset]);
	//UsbTx[offset]	= 0xFF & (InitSpeed >> 24);
	//UsbTx[offset+1] = 0xFF & (InitSpeed >> 16);
	//UsbTx[offset+2] = 0xFF & (InitSpeed >> 8);
	//UsbTx[offset+3] = 0xFF & (InitSpeed >> 0);

	//Speed
	offset += 4;
	Div32(Speed, &UsbTx[offset]);
	//UsbTx[offset]	  =	0xFF & (Speed >> 24);
	//UsbTx[offset + 1] = 0xFF & (Speed >> 16);
	//UsbTx[offset + 2] = 0xFF & (Speed >> 8);
	//UsbTx[offset + 3] = 0xFF & (Speed >> 0);

	//Acceleration
	offset += 4;
	Div32(Acc, &UsbTx[offset]);
	//UsbTx[offset]	  = 0xFF & (Acc >> 24);
	//UsbTx[offset + 1] = 0xFF & (Acc >> 16);
	//UsbTx[offset + 2] = 0xFF & (Acc >> 8);
	//UsbTx[offset + 3] = 0xFF & (Acc >> 0);

	//Jerk
	offset += 4;
	Div32(Jerk, &UsbTx[offset]);
	//UsbTx[offset]	  =	0xFF & (Jerk >> 24);
	//UsbTx[offset + 1] = 0xFF & (Jerk >> 16);
	//UsbTx[offset + 2] = 0xFF & (Jerk >> 8);
	//UsbTx[offset + 3] = 0xFF & (Jerk >> 0);

	//Position
	offset += 4;
	Div32(Pos, &UsbTx[offset]);
	//UsbTx[offset]     =	0xFF & (Pos >> 24);
	//UsbTx[offset + 1] = 0xFF & (Pos >> 16);
	//UsbTx[offset + 2] = 0xFF & (Pos >> 8);
	//UsbTx[offset + 3] = 0xFF & (Pos >> 0);

	//Deceleration
	if (Dec != 0) {
		offset += 4;
		Div32(Dec, &UsbTx[offset]);
		//UsbTx[offset]	  = 0xFF & (Dec >> 24);
		//UsbTx[offset + 1] = 0xFF & (Dec >> 16);
		//UsbTx[offset + 2] = 0xFF & (Dec >> 8);
		//UsbTx[offset + 3] = 0xFF & (Dec >> 0);
	}
	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write
	return 0;
}

INT32 NhgDrvAbs(INT32 InitSpeed, INT32 Speed, INT32 Acc, INT32 Jerk, INT32 Pos, INT32 Dec) {
	if (InitSpeed < 0 || Speed < 0 || Acc < 0 || Jerk < 0 || Dec < 0)
		return -6; //some input can't < 0

	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = DrvRel;

	//UsbTx[2] to UsbTx[17] for reg0 to reg7
	//cmd Reg0 = 0x54 absolute drive
	UsbTx[3] = 0x00;
	UsbTx[4] = 0x54;

	//InitSpeed
	int offset = 19;

	Div32(InitSpeed, &UsbTx[offset]);
	//UsbTx[offset] = 0xFF & (InitSpeed >> 24);
	//UsbTx[offset + 1] = 0xFF & (InitSpeed >> 16);
	//UsbTx[offset + 2] = 0xFF & (InitSpeed >> 8);
	//UsbTx[offset + 3] = 0xFF & (InitSpeed >> 0);

	//Speed
	offset += 4;
	Div32(Speed, &UsbTx[offset]); //23
	//UsbTx[offset] = 0xFF & (Speed >> 24);
	//UsbTx[offset + 1] = 0xFF & (Speed >> 16);
	//UsbTx[offset + 2] = 0xFF & (Speed >> 8);
	//UsbTx[offset + 3] = 0xFF & (Speed >> 0);

	//Acceleration
	offset += 4;
	Div32(Acc, &UsbTx[offset]); //27
	//UsbTx[offset] = 0xFF & (Acc >> 24);
	//UsbTx[offset + 1] = 0xFF & (Acc >> 16);
	//UsbTx[offset + 2] = 0xFF & (Acc >> 8);
	//UsbTx[offset + 3] = 0xFF & (Acc >> 0);

	//Jerk
	offset += 4;
	Div32(Jerk, &UsbTx[offset]); //31
	//UsbTx[offset] = 0xFF & (Jerk >> 24);
	//UsbTx[offset + 1] = 0xFF & (Jerk >> 16);
	//UsbTx[offset + 2] = 0xFF & (Jerk >> 8);
	//UsbTx[offset + 3] = 0xFF & (Jerk >> 0);

	//Position
	offset += 4;
	Div32(Pos, &UsbTx[offset]); //35
	//UsbTx[offset] = 0xFF & (Pos >> 24);
	//UsbTx[offset + 1] = 0xFF & (Pos >> 16);
	//UsbTx[offset + 2] = 0xFF & (Pos >> 8);
	//UsbTx[offset + 3] = 0xFF & (Pos >> 0);

	//Deceleration
	if (Dec != 0) {
		offset += 4;
		Div32(Dec, &UsbTx[offset]); //39
		//UsbTx[offset] = 0xFF & (Dec >> 24);
		//UsbTx[offset + 1] = 0xFF & (Dec >> 16);
		//UsbTx[offset + 2] = 0xFF & (Dec >> 8);
		//UsbTx[offset + 3] = 0xFF & (Dec >> 0);
	}
	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write
	return 0;
}

INT32 NhgVelDrv(INT32 Speed) {

	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = VelDrive;

	//Speed
	Div32(Speed, &UsbTx[23]);

	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write
	return 0;
}

INT32 NhgJogDrv(INT32 Speed, INT32 Acc, INT32 Dec) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = JogDrive;

	//Speed
	Div32(Speed, &UsbTx[23]);
	Div32(Acc, &UsbTx[27]); //27
	if (Dec != 0)
		Div32(Dec, &UsbTx[39]); //39

	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write
	return 0;
}

INT32 NhgMoveDecStop() {
	UINT8  UsbTx[256];
	//clear TX buffer
	std::fill_n(UsbTx, 256, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = DecStp;

	//UsbTx[2] to UsbTx[17] for reg0 to reg7
	//cmd Reg0 = 0x56 decelarate stop
	UsbTx[3] = 0x00;
	UsbTx[4] = 0x56;

	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write
	return 0;
}

INT32 NhgMoveInsStop() {
	UINT8  UsbTx[256];
	//clear TX buffer
	std::fill_n(UsbTx, 256, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = InsStp;

	//UsbTx[2] to UsbTx[17] for reg0 to reg7
	//cmd Reg0 = 0x57 instant stop
	UsbTx[3] = 0x00;
	UsbTx[4] = 0x57;

	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write
	return 0;
}

INT32 NhgLedCtr(UINT16 LEDs) {
	UINT8  UsbTx[256];
	//clear TX buffer
	std::fill_n(UsbTx, 256, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = LedCtr;

	UsbTx[36] = 0xFF & (LEDs >> 8);
	UsbTx[37] = 0xFF & (LEDs);

	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write
	return 0;
}

INT32 NhgEcho(INT32* InitSpeed, INT32* Speed, INT32* Acc, INT32* Jerk, INT32* Pos, INT32* Dec) {
	UINT8  UsbRx[64];
	//clear TX buffer
	std::fill_n(UsbRx, 64, 0);
	res = hid_read(handle, UsbRx, 64);

	//InitSpeed
	int offset = 19;
	*InitSpeed = (UsbRx[offset] << 24) ^ (UsbRx[offset + 1] << 16) ^ (UsbRx[offset + 2] << 8) ^ UsbRx[offset + 3];

	//Speed
	offset += 4;
	*Speed = (UsbRx[offset] << 24) ^ (UsbRx[offset + 1] << 16) ^ (UsbRx[offset + 2] << 8) ^ UsbRx[offset + 3];

	//Acceleration
	offset += 4;
	*Acc = (UsbRx[offset] << 24) ^ (UsbRx[offset + 1] << 16) ^ (UsbRx[offset + 2] << 8) ^ UsbRx[offset + 3];

	//Jerk
	offset += 4;
	*Jerk = (UsbRx[offset] << 24) ^ (UsbRx[offset + 1] << 16) ^ (UsbRx[offset + 2] << 8) ^ UsbRx[offset + 3];

	//Position
	offset += 4;
	*Pos = (UsbRx[offset] << 24) ^ (UsbRx[offset + 1] << 16) ^ (UsbRx[offset + 2] << 8) ^ UsbRx[offset + 3];

	//Deceleration
	if (*Dec != 0) {
		offset += 4;
		*Dec = (UsbRx[offset] << 24) ^ (UsbRx[offset + 1] << 16) ^ (UsbRx[offset + 2] << 8) ^ UsbRx[offset + 3];
	}
	return 0;
}

INT32 NhgStatusGetBasDrv(UINT32* CrlPos, UINT32* RealPos, UINT32* CrlVel) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = GetPos;

	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write

	UINT8  UsbRx[64];
	//clear RX buffer
	std::fill_n(UsbRx, 64, 0);
	res = 0;
	while (res == 0) {
		res = hid_read(handle, UsbRx, 64);
		if (res == 0) {}
		//do nothing, waiting
		if (res < 0)
			return -5;
	}

	unsigned long lp, rp, cv;

	lp = UsbRx[0];
	lp = (lp << 8) | UsbRx[1];
	lp = (lp << 8) | UsbRx[2];
	lp = (lp << 8) | UsbRx[3];

	rp = UsbRx[4];
	rp = (rp << 8) | UsbRx[5];
	rp = (rp << 8) | UsbRx[6];
	rp = (rp << 8) | UsbRx[7];

	cv = UsbRx[8];
	cv = (cv << 8) | UsbRx[9];
	cv = (cv << 8) | UsbRx[10];
	cv = (cv << 8) | UsbRx[11];
	*CrlPos = lp;
	*RealPos = rp;
	*CrlVel = cv;
	return 0;
}

INT32 NhgStatusGetActPos(UINT32* RealPos) {
	UINT32 LP, CV;
	return NhgStatusGetBasDrv(&LP, RealPos, &CV);
}

INT32 NhgStatusGetCmdPos(UINT32* CrlPos) {
	UINT32 RP, CV;
	return NhgStatusGetBasDrv(CrlPos, &RP, &CV);
}


INT32 NhgMotGetParaLoad(UINT32* InitPos, UINT32* InitVel, UINT32* InitAcc) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = GetDrvPara;

	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't write

	UINT8  UsbRx[64];
	//clear RX buffer
	std::fill_n(UsbRx, 64, 0);
	res = 0;
	while (res == 0) {
		res = hid_read(handle, UsbRx, 64);
		if (res == 0) {}
		//do nothing, waiting
		if (res < 0)
			return -5;
	}

	unsigned long inlp, incv, intac;

	intac = UsbRx[0];
	intac = (intac << 8) | UsbRx[1];
	intac = (intac << 8) | UsbRx[2];
	intac = (intac << 8) | UsbRx[3];

	incv = UsbRx[4];
	incv = (incv << 8) | UsbRx[5];
	incv = (incv << 8) | UsbRx[6];
	incv = (incv << 8) | UsbRx[7];

	inlp = UsbRx[8];
	inlp = (inlp << 8) | UsbRx[9];
	inlp = (inlp << 8) | UsbRx[10];
	inlp = (inlp << 8) | UsbRx[11];

	*InitPos = inlp;
	*InitVel = incv;
	*InitAcc = intac;
	return 0;
}

INT32 NhgSetLpRp(INT32 LogicalPos, INT32 RealPos) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = SetLpRp;

	Div32(LogicalPos, &UsbTx[5]);
	Div32(RealPos, &UsbTx[9]);

	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't acess

	return 0;
}

INT32 NhgSetLp(INT32 LogicalPos) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = SetLp;

	Div32(LogicalPos, &UsbTx[5]);

	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't acess

	return 0;
}

INT32 NhgSetRp(INT32 RealPos) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = SetRp;

	Div32(RealPos, &UsbTx[9]);

	res = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't acess

	return 0;
}

//if ChangeValue = 0 => ChangeValue = 1.
INT32 NhgChangeVel(INT32 ChangeValue) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = ChangeVel;

	if (ChangeValue >= 0) {
		UsbTx[3] = 0x00;
		UsbTx[4] = 0x70; //cmd increase
	}
	else {
		UsbTx[3] = 0x00;
		UsbTx[4] = 0x71; //cmd decrease
	}
	Div32(abs(ChangeValue), &UsbTx[5]);

	res = hid_write(handle, UsbTx, 64);

	if (res < 0)	return -5; //can't acess

	return 0;
}

INT32 NhgSpdChg(INT32 ChangeValue) {

	return 0;
}

INT32 NhgDrvWait() {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = UsbDrvWait;

		 = hid_write(handle, UsbTx, 64);
	if (res < 0)
		return -5; //can't acess

	UINT8  UsbRx[64];
	//clear TX buffer
	std::fill_n(UsbRx, 64, 0);
	res = 0;
	while (res == 0) {
		res = hid_read(handle, UsbRx, 64);
		if (res == 0) {}
		//do nothing, waiting
		if (res < 0)
			return -5;
	}

	return 0x01 & UsbRx[1];
}

INT32 NhgSpdInc(INT32 IncreaseValue) {

	return 0;
}

INT32 NhgSpdDec(INT32 DecreaseValue) {

	return 0;
}

void Nhgtestfunc(INT32 test_var) {
	UINT8  UsbTx[64];
	//clear TX buffer
	std::fill_n(UsbTx, 64, 0);
	//report byte
	UsbTx[0] = 0x01;

	//USB user define cmd
	UsbTx[1] = 0x00;
	UsbTx[2] = 0x10;


	res = hid_write(handle, UsbTx, 64);
	//if (res < 0)
	//	return -5; //can't acess

	//return 0;
}