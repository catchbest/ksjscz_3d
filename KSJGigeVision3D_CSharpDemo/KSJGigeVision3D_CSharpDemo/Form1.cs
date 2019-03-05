using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using KSJGigeVisionApi_Space;
using System.Threading;
using System.IO;
using System.Drawing.Imaging;

namespace KSJGigeVision3D_CSharpDemo
{
    public struct Mystr
    {
        public uint i;
    };
    public partial class Form1 : Form
    {
        int[,] CamareIndex = new int[64, 2];
        int m_nDeviceCurSel;
        public Form1()
        {
            InitializeComponent();
            KSJGigeVisionApi.KSJGIGEVISION_Init();

            int nCount = 0;
            int nAdapterIndex = KSJGigeVisionApi.KSJGIGEVISION_AdapterGetCount();
            int nCameraIndex = 0;
            for (int i = 0; i < nAdapterIndex; i++)
            {
                nCameraIndex = KSJGigeVisionApi.KSJGIGEVISION_CameraGetCount(i);
                for (int j = 0; j < nCameraIndex; j++)
                {
                    CamareIndex[nCount,0] = i;
                    CamareIndex[nCount,1] = j;
                    nCount++;
                }
            }

            label1.Text = String.Format("{0} Device Found:", nCount);

            for (int i = 0; i < nCount; i++)//获取每个相机的参数插入到顶部下拉框中
            {
                string szText = String.Format("camera{0}", i + 1);
                comboBox_DEVICE_LIST.Items.Insert(i, szText);
            }

            m_nDeviceCurSel = -1;
            exitEvent = new AutoResetEvent(false);
            
        }


        private void UpdateInterface()
        {
            if (m_nDeviceCurSel == -1) return;

            uint nColSize = 0;
	        uint nRowSize = 0;
	        uint nType = 0;

            KSJGigeVisionApi.KSJGIGEVISION_ReadRegister(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], 0x100, ref nColSize);
            KSJGigeVisionApi.KSJGIGEVISION_ReadRegister(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], 0x104, ref nRowSize);
            KSJGigeVisionApi.KSJGIGEVISION_ReadRegister(CamareIndex[m_nDeviceCurSel,0], CamareIndex[m_nDeviceCurSel,1], 0xb000, ref nType);
            CSnumericUpDown.Value = nColSize;
            RSnumericUpDown.Value = nRowSize;
            TYPEnumericUpDown.Value = nType;
        }

        private void comboBox_DEVICE_LIST_SelectedIndexChanged(object sender, EventArgs e)
        {
            m_nDeviceCurSel = comboBox_DEVICE_LIST.SelectedIndex;
            UpdateInterface();
        }

        private AutoResetEvent exitEvent;
        private Thread thread;
        private void PreviewcheckBox_CheckedChanged(object sender, EventArgs e)
        {
            bool bCheck = PreviewcheckBox.Checked;
            if (bCheck)
            {
                thread = new Thread(new ThreadStart(GvspThread));
                thread.Start();
            }
            else
            {
                exitEvent.Set();
                thread.Join();
            }
        }

        /// <summary>  
        /// 将一个字节数组转换为8bit灰度位图  
        /// </summary>  
        /// <param name="rawValues">显示字节数组</param>  
        /// <param name="width">图像宽度</param>  
        /// <param name="height">图像高度</param>  
        /// <returns>位图</returns>  
        public static Bitmap ToGrayBitmap(byte[] rawValues, int width, int height)
        {
            //// 申请目标位图的变量，并将其内存区域锁定  
            Bitmap bmp = new Bitmap(width, height, PixelFormat.Format8bppIndexed);
            BitmapData bmpData = bmp.LockBits(new Rectangle(0, 0, width, height),
             ImageLockMode.WriteOnly, PixelFormat.Format8bppIndexed);

            //// 获取图像参数  
            int stride = bmpData.Stride;  // 扫描线的宽度  
            int offset = stride - width;  // 显示宽度与扫描线宽度的间隙  
            IntPtr iptr = bmpData.Scan0;  // 获取bmpData的内存起始位置  
            int scanBytes = stride * height;// 用stride宽度，表示这是内存区域的大小  

            //// 下面把原始的显示大小字节数组转换为内存中实际存放的字节数组  
            int posScan = 0, posReal = 0;// 分别设置两个位置指针，指向源数组和目标数组  
            byte[] pixelValues = new byte[scanBytes];  //为目标数组分配内存  

            for (int x = 0; x < height; x++)
            {
                //// 下面的循环节是模拟行扫描  
                for (int y = 0; y < width; y++)
                {
                    pixelValues[posScan++] = rawValues[posReal++];
                }
                posScan += offset;  //行扫描结束，要将目标位置指针移过那段“间隙”  
            }

            //// 用Marshal的Copy方法，将刚才得到的内存字节数组复制到BitmapData中  
            System.Runtime.InteropServices.Marshal.Copy(pixelValues, 0, iptr, scanBytes);
            bmp.UnlockBits(bmpData);  // 解锁内存区域  

            //// 下面的代码是为了修改生成位图的索引表，从伪彩修改为灰度  
            ColorPalette tempPalette;
            using (Bitmap tempBmp = new Bitmap(1, 1, PixelFormat.Format8bppIndexed))
            {
                tempPalette = tempBmp.Palette;
            }
            for (int i = 0; i < 256; i++)
            {
                tempPalette.Entries[i] = Color.FromArgb(i, i, i);
            }

            bmp.Palette = tempPalette;

            //// 算法到此结束，返回结果  
            return bmp;
        }  

        private void GvspThread()
        {
            int nRet = 0;
            uint uiGvspport = 0xcbbb;
            uint uiGvspsize = 0;
            KSJGigeVisionApi.KSJGIGEVISION_WriteRegister(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], 0xd00, uiGvspport);
            KSJGigeVisionApi.KSJGIGEVISION_CameraSetGvspPort(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], (int)uiGvspport);
            KSJGigeVisionApi.KSJGIGEVISION_ReadRegister(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], 0xd04, ref uiGvspsize);
            KSJGigeVisionApi.KSJGIGEVISION_CameraSetGvspSize(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], (int)uiGvspsize);

            uint nColSize = 0;
            uint nRowSize = 0;
            uint nType = 0;
            KSJGigeVisionApi.KSJGIGEVISION_ReadRegister(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], 0x100, ref nColSize);
            KSJGigeVisionApi.KSJGIGEVISION_ReadRegister(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], 0x104, ref nRowSize);
            KSJGigeVisionApi.KSJGIGEVISION_ReadRegister(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], 0xb000, ref nType);
            byte[] pImageData = new byte[nColSize * nRowSize * 8];
            KSJGigeVisionApi.KSJGIGEVISION_WriteRegister(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], 0x124, 1);
            decimal fProfiley;
            fProfiley = YnumericUpDown.Value;
            float fZmapLow = (float)(Math.Round(LownumericUpDown.Value * 100)) / 100;
            float fZmapHigh = (float)(Math.Round(HighnumericUpDown.Value * 100)) / 100;
            float unit = (fZmapHigh - fZmapLow) / 255;
            int nWidth = (int)nColSize;
            int nHeight = (int)nRowSize;
            int nCount = 0;
            float[] profile;
            float[] profilex;
            profile = new float[nColSize * nRowSize];
            profilex = new float[nColSize * nRowSize];
            Graphics g;
            int i = 0;
            Point p1 = new Point(0, 0);
            Point p2 = new Point(0, 0);
            int Index = 0;
            byte[] idata = new byte[nColSize * nRowSize];
            Bitmap bitmap;
           
            while (true)
            {
                if (exitEvent.WaitOne(0))
                {
                    KSJGigeVisionApi.KSJGIGEVISION_WriteRegister(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], 0x124, 0);
                    break;
                }

                nRet = KSJGigeVisionApi.KSJGIGEVISION_CameraGet3DGvspData(CamareIndex[m_nDeviceCurSel, 0], CamareIndex[m_nDeviceCurSel, 1], (int)nType, pImageData, ref nWidth, ref nHeight);//采集相机一帧
                if (nRet == 1)//采集成功
                {
                    if (nType == 0)//gray
                    {
                        bitmap = ToGrayBitmap(pImageData, nWidth, nHeight);
                        this.PictureBox_PREVIEWWND.Image = bitmap;
                    }
                    else if (nType == 1)//profile
                    {
                        for (i = 0; i < nWidth; i++) profile[i] = BitConverter.ToSingle(pImageData, i * 4);
                        for (i = 0; i < nWidth; i++) profilex[i] = BitConverter.ToSingle(pImageData, (i + nWidth) * 4);
                        bitmap = new Bitmap(nWidth, 256);
                        g = Graphics.FromImage(bitmap);
                        g.Clear(Color.Black);
                        for (i = 1; i < nWidth; ++i)//绘制图片
                        {
                            if (profile[i - 1] != -1000 && profile[i] != -1000 && profilex[i - 1] > 0 && profilex[i] > 0)
                            {
                                p1.X = (int)(profilex[i - 1] * 1280 / 50);
                                p1.Y = (int)(256 - profile[i - 1] * 256 / 10);
                                p2.X = (int)(profilex[i] * 1280 / 50);
                                p2.Y = (int)(256 - profile[i] * 256 / 10);
                                g.DrawLine(new Pen(Color.White, 1), p1, p2);
                            }
                        }

                        this.PictureBox_PREVIEWWND.Image = bitmap;
                        g.Dispose();
                    }
                    else if (nType == 2)//3d
                    {
                        nCount = nWidth * nHeight;
                        for (i = 0; i < nCount; i++) profile[i] = BitConverter.ToSingle(pImageData, i * 4);
                        for (i = 0; i < nCount; i++) profilex[i] = BitConverter.ToSingle(pImageData, (i + nCount) * 4);

                        Index = 0;
                        for (i = 0; i < nHeight; ++i)
                        {
                            for (int j = 0; j < nWidth; ++j)
                            {
                                if (profile[Index] <= fZmapHigh && profile[Index] >= fZmapLow)
                                {
                                    idata[Index] = (byte)((profile[Index] - fZmapLow) / unit + 0.5);
                                }
                                else
                                {
                                    idata[Index] = 0;
                                }

                                ++Index;
                            }
                        }

                        bitmap = ToGrayBitmap(idata, nWidth, nHeight);
                        this.PictureBox_PREVIEWWND.Image = bitmap;

                        bool bCheck = SavecheckBox.Checked;
                        if (bCheck)
                        {
                            FileStream fs = new FileStream("test.pcd", FileMode.Create);
                            string temp = "# .PCD v0.7 - Point Cloud Data file format\n";
                            byte[] data = System.Text.Encoding.Default.GetBytes(temp);
                            fs.Write(data, 0, data.Length);
                            temp = "VERSION 0.7\n";
                            data = System.Text.Encoding.Default.GetBytes(temp);
                            fs.Write(data, 0, data.Length);
                            temp = "FIELDS x y z\n";
                            data = System.Text.Encoding.Default.GetBytes(temp);
                            fs.Write(data, 0, data.Length);
                            temp = "SIZE 4 4 4\n";
                            data = System.Text.Encoding.Default.GetBytes(temp);
                            fs.Write(data, 0, data.Length);
                            temp = "TYPE F F F\n";
                            data = System.Text.Encoding.Default.GetBytes(temp);
                            fs.Write(data, 0, data.Length);
                            temp = "COUNT 1 1 1\n";
                            data = System.Text.Encoding.Default.GetBytes(temp);
                            fs.Write(data, 0, data.Length);
                            temp = string.Format("WIDTH {0}\n", nColSize);
                            data = System.Text.Encoding.Default.GetBytes(temp);
                            fs.Write(data, 0, data.Length);
                            temp = string.Format("HEIGHT {0}\n", nRowSize);
                            data = System.Text.Encoding.Default.GetBytes(temp);
                            fs.Write(data, 0, data.Length);
                            temp = "VIEWPOINT 0 0 0 1 0 0 0\n";
                            data = System.Text.Encoding.Default.GetBytes(temp);
                            fs.Write(data, 0, data.Length);
                            temp = string.Format("POINTS {0}\n", nCount);
                            data = System.Text.Encoding.Default.GetBytes(temp);
                            fs.Write(data, 0, data.Length);
                            temp = "DATA ascii\n";
                            data = System.Text.Encoding.Default.GetBytes(temp);
                            fs.Write(data, 0, data.Length);

                            for (i = 0; i < nCount; i++)
                            {
                                if (i != nCount - 1)
                                {
                                    if (profilex[i] == -1000) temp = "nan\n";
                                    else temp = string.Format("{0:N5} {1:N5} {2:N5}\n", profilex[i], fProfiley * (i / 1280), profile[i]);
                                }
                                else
                                {
                                    if (profilex[i] == -1000) temp = "nan";
                                    else temp = string.Format("{0:N5} {1:N5} {2:N5}", profilex[i], fProfiley * (i / 1280), profile[i]);  
                                }

                                data = System.Text.Encoding.Default.GetBytes(temp);
                                fs.Write(data, 0, data.Length);
                            }
                            //清空缓冲区、关闭流
                            fs.Flush();
                            fs.Close();
                        }
                    }
                }
            }
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            try
            {
                KSJGigeVisionApi.KSJGIGEVISION_UnInit();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }


            Application.Exit();
        }

    }
}
