using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.Capture.Frames;
using Windows.Devices.Enumeration;
using Windows.Media.Capture;
using Windows.UI.Xaml.Media.Imaging;
using Windows.Media.MediaProperties;
using Windows.Graphics.Imaging;
using System.Threading;
using Windows.UI.Core;
using Windows.Media.Core;
using System.Diagnostics;
using Windows.Media;
using Windows.Media.Devices;
using Windows.Media.Audio;

namespace NASA_Suits
{
    class Hololens_capture
    {
        static void Main(string[] args)
        {
            //**SEARCH FOR COLOR VIDEO STREAM SOURCE**//
            var frameSourceGroups = await MediaFrameSourceGroup.FindAllAsync();

            MediaFrameSourceGroup selectedGroup = null;
            MediaFrameSourceInfo colorSourceInfo = null;

            foreach (var sourceGroup in frameSourceGroups)
            {
                foreach (var sourceInfo in sourceGroup.SourceInfos)
                {
                    if (sourceInfo.MediaStreamType == MediaStreamType.VideoPreview
                        && sourceInfo.SourceKind == MediaFrameSourceKind.Color)
                    {
                        colorSourceInfo = sourceInfo;
                        break;
                    }
                }
                if (colorSourceInfo != null)
                {
                    selectedGroup = sourceGroup;
                    break;
                }
            }

            //**INITIATE MEDIAFRAMECAPTURE TO CAPTURE FRAMES**//
            MediaCapture mediaCapture;

            mediaCapture = new MediaCapture();

            var settings = new MediaCaptureInitializationSettings()
            {
                SourceGroup = selectedGroup,
                SharingMode = MediaCaptureSharingMode.ExclusiveControl,
                MemoryPreference = MediaCaptureMemoryPreference.Cpu,
                StreamingCaptureMode = StreamingCaptureMode.Video
            };
            try
            {
                await mediaCapture.InitializeAsync(settings);
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine("MediaCapture initialization failed: " + ex.Message);
                return;
            }

            var colorFrameSource = mediaCapture.FrameSources[colorSourceInfo.Id];
            /*
            var preferredFormat = colorFrameSource.SupportedFormats.Where(format =>
            {
                return format.VideoFormat.Width >= 1080
                && format.Subtype == MediaEncodingSubtypes.Argb32;

            }).FirstOrDefault();

            if (preferredFormat == null)
            {
                // Our desired format is not supported
                return;
            }

            await colorFrameSource.SetFormatAsync(preferredFormat);
            */

            //**CREATE A FRAME READER**//
            BitmapSize size = new BitmapSize() // Choose a lower resolution to make the image processing more performant
            {
                Height = 480,
                Width = 640
            };

            mediaFrameReader = await mediaCapture.CreateFrameReaderAsync(colorFrameSource, MediaEncodingSubtypes.Argb32, size);
            mediaFrameReader.FrameArrived += ColorFrameReader_FrameArrived_OpenCV;

            imageElement.Source = new SoftwareBitmapSource();
            _frameRenderer = new FrameRenderer(imageElement);

            await mediaFrameReader.StartAsync();


        }
        private void ColorFrameReader_FrameArrived_OpenCV(MediaFrameReader sender, MediaFrameArrivedEventArgs args)
        {

            var mediaFrameReference = sender.TryAcquireLatestFrame();
            if (mediaFrameReference != null)
            {

                SoftwareBitmap openCVInputBitmap = null;
                var inputBitmap = mediaFrameReference.VideoMediaFrame?.SoftwareBitmap;
                if (inputBitmap != null)
                {
                    //The XAML Image control can only display images in BRGA8 format with premultiplied or no alpha
                    if (inputBitmap.BitmapPixelFormat == BitmapPixelFormat.Bgra8
                        && inputBitmap.BitmapAlphaMode == BitmapAlphaMode.Premultiplied)
                    {
                        openCVInputBitmap = SoftwareBitmap.Copy(inputBitmap);
                    }
                    else
                    {
                        openCVInputBitmap = SoftwareBitmap.Convert(inputBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
                    }

                    SoftwareBitmap openCVOutputBitmap = new SoftwareBitmap(BitmapPixelFormat.Bgra8, openCVInputBitmap.PixelWidth, openCVInputBitmap.PixelHeight, BitmapAlphaMode.Premultiplied);

                    // operate on the image and render it
                    openCVHelper.Blur(openCVInputBitmap, openCVOutputBitmap);
                    _frameRenderer.PresentSoftwareBitmap(openCVOutputBitmap);

                }
            }
        }
    }
}
