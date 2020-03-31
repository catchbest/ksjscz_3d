#define halcon12//有这个定义时加载halcon，屏蔽掉则不加载，保存pcd文件
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
using HalconDotNet;
using System.Runtime.InteropServices;

namespace KSJGigeVision3D_CSharpDemo
{
    public partial class Form1 : Form
    {
        Point m_ptCanvas;           //画布原点在设备上的坐标
        Point m_ptCanvasBuf;        //重置画布坐标计算时用的临时变量
        Point m_ptBmp;              //图像位于画布坐标系中的坐标
        float m_nScale = 1.0F;      //缩放比例
        Point m_ptMouseDown;        //鼠标点下是在设备坐标上的坐标
        Bitmap bitmap;//预览位图
        bool init = false;
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

            m_nDeviceCurSel = 0;
            comboBox_DEVICE_LIST.SelectedIndex = m_nDeviceCurSel;
            exitEvent = new AutoResetEvent(false);
            this.PictureBox_PREVIEWWND.MouseWheel += new MouseEventHandler(PictureBox_PREVIEWWND_MouseWheel);//控件滚轮事件
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
            YnumericUpDown.Value = 0.1M;
            LownumericUpDown.Value = 0M;
            HighnumericUpDown.Value = 20M;
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
            float[] profiley;
            profile = new float[nColSize * nRowSize];
            profilex = new float[nColSize * nRowSize];
            profiley = new float[nColSize * nRowSize];
            Graphics g;
            int i = 0;
            Point p1 = new Point(0, 0);
            Point p2 = new Point(0, 0);
            int Index = 0;
            byte[] idata = new byte[nColSize * nRowSize];
            //Bitmap bitmap;
           
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
                        if (!init)//未进行过宽高比例计算
                        {
                            float proportion = (float)PictureBox_PREVIEWWND.Width / (float)PictureBox_PREVIEWWND.Height;
                            float proportion_bmp = (float)nWidth / (float)nHeight;
                            if (proportion < proportion_bmp)//比较控件和图片宽高比，根据不同情况设置位图在控件内初始显示位置
                            {
                                m_nScale = (float)PictureBox_PREVIEWWND.Width / (float)nWidth;
                                float h = nHeight * m_nScale;
                                m_ptCanvas = new Point(0, 0);
                                int offset = (PictureBox_PREVIEWWND.Height - (int)h) / 2;
                                m_ptBmp = new Point(0, (int)(offset / m_nScale));
                            }
                            else
                            {
                                m_nScale = (float)PictureBox_PREVIEWWND.Height / (float)nHeight;
                                float w = nWidth * m_nScale;
                                m_ptCanvas = new Point(0, 0);
                                int offset = (PictureBox_PREVIEWWND.Width - (int)w) / 2;
                                m_ptBmp = new Point((int)(offset / m_nScale), 0);
                            }
                            init = true;//进行设置后标记
                        }
                        PictureBox_PREVIEWWND.Invalidate();
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

                        if (!init)//未进行过宽高比例计算
                        {
                            float proportion = (float)PictureBox_PREVIEWWND.Width / (float)PictureBox_PREVIEWWND.Height;
                            float proportion_bmp = (float)nWidth / (float)nHeight;
                            if (proportion < proportion_bmp)//比较控件和图片宽高比，根据不同情况设置位图在控件内初始显示位置
                            {
                                m_nScale = (float)PictureBox_PREVIEWWND.Width / (float)nWidth;
                                float h = nHeight * m_nScale;
                                m_ptCanvas = new Point(0, 0);
                                int offset = (PictureBox_PREVIEWWND.Height - (int)h) / 2;
                                m_ptBmp = new Point(0, (int)(offset / m_nScale));
                            }
                            else
                            {
                                m_nScale = (float)PictureBox_PREVIEWWND.Height / (float)nHeight;
                                float w = nWidth * m_nScale;
                                m_ptCanvas = new Point(0, 0);
                                int offset = (PictureBox_PREVIEWWND.Width - (int)w) / 2;
                                m_ptBmp = new Point((int)(offset / m_nScale), 0);
                            }
                            init = true;//进行设置后标记
                        }
                        PictureBox_PREVIEWWND.Invalidate();
                        //this.PictureBox_PREVIEWWND.Image = bitmap;
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
                        //this.PictureBox_PREVIEWWND.Image = bitmap;
                        if (!init)//未进行过宽高比例计算
                        {
                            float proportion = (float)PictureBox_PREVIEWWND.Width / (float)PictureBox_PREVIEWWND.Height;
                            float proportion_bmp = (float)nWidth / (float)nHeight;
                            if (proportion < proportion_bmp)//比较控件和图片宽高比，根据不同情况设置位图在控件内初始显示位置
                            {
                                m_nScale = (float)PictureBox_PREVIEWWND.Width / (float)nWidth;
                                float h = nHeight * m_nScale;
                                m_ptCanvas = new Point(0, 0);
                                int offset = (PictureBox_PREVIEWWND.Height - (int)h) / 2;
                                m_ptBmp = new Point(0, (int)(offset / m_nScale));
                            }
                            else
                            {
                                m_nScale = (float)PictureBox_PREVIEWWND.Height / (float)nHeight;
                                float w = nWidth * m_nScale;
                                m_ptCanvas = new Point(0, 0);
                                int offset = (PictureBox_PREVIEWWND.Width - (int)w) / 2;
                                m_ptBmp = new Point((int)(offset / m_nScale), 0);
                            }
                            init = true;//进行设置后标记
                        }
                        PictureBox_PREVIEWWND.Invalidate();
                        bool bCheck = SavecheckBox.Checked;
                        if (bCheck)
                        {
#if halcon12//使用halcon保存om3文件
                            string t1, filename;
                            t1 = DateTime.Now.ToString("yyyy-MM-dd");
                            System.Diagnostics.Debug.WriteLine("开始保存");
                            filename = t1 + "-" + DateTime.Now.Hour.ToString() + DateTime.Now.Minute.ToString() + DateTime.Now.Second.ToString() + ".om3";
                            HObject Hobjx = null, Hobjy = null, Hobjz = null;
                            IntPtr ptrx = Marshal.AllocHGlobal(nCount*4);
                            Marshal.Copy(profilex, 0, ptrx, nCount);
                            IntPtr ptrz = Marshal.AllocHGlobal(nCount * 4);
                            Marshal.Copy(profile, 0, ptrz, nCount);
                            for (i = 0; i < nCount; i++)
                            {
                                profiley[i] = (float)fProfiley * (i / 1280);
                            }

                            IntPtr ptry = Marshal.AllocHGlobal(nCount * 4);
                            Marshal.Copy(profiley, 0, ptrz, nCount);
                            HOperatorSet.GenImage1(out Hobjx, "real", nWidth, nHeight, ptrx);
                            HOperatorSet.GenImage1(out Hobjy, "real", nWidth, nHeight, ptry);
                            HOperatorSet.GenImage1(out Hobjz, "real", nWidth, nHeight, ptrz);
                            HTuple hv_ObjectModel3D;
                            HOperatorSet.XyzToObjectModel3d(Hobjx, Hobjy, Hobjz, out hv_ObjectModel3D);
                            HOperatorSet.WriteObjectModel3d(hv_ObjectModel3D, "om3", filename, "invert_normals", "false");
                            System.Diagnostics.Debug.WriteLine("保存完毕");
#else//不使用halcon保存pcd文件
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
                                    if (profilex[i] == -1000) temp = "nan nan nan\n";
                                    else temp = string.Format("{0:N5} {1:N5} {2:N5}\n", profilex[i], fProfiley * (i / 1280), profile[i]);
                                }
                                else
                                {
                                    if (profilex[i] == -1000) temp = "nan nan nan";
                                    else temp = string.Format("{0:N5} {1:N5} {2:N5}", profilex[i], fProfiley * (i / 1280), profile[i]);  
                                }

                                data = System.Text.Encoding.Default.GetBytes(temp);
                                fs.Write(data, 0, data.Length);
                            }
                            //清空缓冲区、关闭流
                            fs.Flush();
                            fs.Close();
#endif
                        }
                    }
                }
            }
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            try
            {
                bool bCheck = PreviewcheckBox.Checked;
                if (bCheck)
                {
                    PreviewcheckBox.Checked = false; 
                }

                KSJGigeVisionApi.KSJGIGEVISION_UnInit();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }


            Application.Exit();
        }

        private void PictureBox_PREVIEWWND_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {      //如果左键点下    初始化计算要用的临时数据
                m_ptMouseDown = e.Location;
                m_ptCanvasBuf = m_ptCanvas;
            }
            PictureBox_PREVIEWWND.Focus();
        }

        private void PictureBox_PREVIEWWND_MouseWheel(object sender, MouseEventArgs e)
        {
            if (m_nScale <= 0.3 && e.Delta <= 0) return;        //缩小下线
            if (m_nScale >= 4.9 && e.Delta >= 0) return;        //放大上线
            //获取 当前点到画布坐标原点的距离
            SizeF szSub = (Size)m_ptCanvas - (Size)e.Location;
            //当前的距离差除以缩放比还原到未缩放长度
            float tempX = szSub.Width / m_nScale;           //这里
            float tempY = szSub.Height / m_nScale;          //将画布比例
            //还原上一次的偏移                               //按照当前缩放比还原到
            m_ptCanvas.X -= (int)(szSub.Width - tempX);     //没有缩放
            m_ptCanvas.Y -= (int)(szSub.Height - tempY);    //的状态
            //重置距离差为  未缩放状态                       
            szSub.Width = tempX;
            szSub.Height = tempY;
            m_nScale += e.Delta > 0 ? 0.2F : -0.2F;
            //重新计算 缩放并 重置画布原点坐标
            m_ptCanvas.X += (int)(szSub.Width * m_nScale - szSub.Width);
            m_ptCanvas.Y += (int)(szSub.Height * m_nScale - szSub.Height);
            PictureBox_PREVIEWWND.Invalidate();
        }

        private void PictureBox_PREVIEWWND_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {      //移动过程中 左键点下 重置画布坐标系
                m_ptCanvas = (Point)((Size)m_ptCanvasBuf + ((Size)e.Location - (Size)m_ptMouseDown));
                PictureBox_PREVIEWWND.Invalidate();
            }
        }

        private void PictureBox_PREVIEWWND_Paint(object sender, PaintEventArgs e)
        {
            if (init)
            {
                Graphics g = e.Graphics;
                g.TranslateTransform(m_ptCanvas.X, m_ptCanvas.Y);       //设置坐标偏移
                g.ScaleTransform(m_nScale, m_nScale);                   //设置缩放比
                g.DrawImage(bitmap, m_ptBmp);
            }
        }

    }
}
