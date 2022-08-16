

//PKGCONFIG = gstreamer-1.0 gstreamer-video-1.0 x11 json-glib-1.0 gstreamer-audio-1.0
 
//main.cpp文件
 
#include <gst/gst.h>
#include <gst/audio/audio.h>
#include <string.h>
 
#define CHUNK_SIZE 1024   /* Amount of bytes we are sending in each buffer */
#define SAMPLE_RATE 44100 /* Samples per second we are sending */
 
/* Structure to contain all our information, so we can pass it to callbacks */
typedef struct _CustomData {
  GstElement *pipeline, *app_source, *tee, *audio_queue, *audio_convert1, *audio_resample, *audio_sink;
  GstElement *video_queue, *audio_convert2, *visual, *video_convert, *video_sink;
  /* Appsink is a sink plugin that supports many different methods for making the application
   * get a handle on the GStreamer data in a pipeline.
   */
  GstElement *app_queue, *app_sink; 
 
  guint64 num_samples;   /* Number of samples generated so far (for timestamp generation) */
  gfloat a, b, c, d;     /* For waveform generation */
 
  guint sourceid;        /* To control the GSource */
 
  GMainLoop *main_loop;  /* GLib's Main Loop */
} CustomData;
 
/* 这是提供 appsrc 的函数。 GLib 将在我们无法控制的时间和速率下调用它，但我们知道我们将在其工作完成时禁
 * 用它（当 appsrc 中的队列已满时）。
 */
 
/* This method is called by the idle GSource in the mainloop, to feed CHUNK_SIZE bytes into appsrc.
 * The idle handler is added to the mainloop when appsrc requests us to start sending data (need-data signal)
 * and is removed when appsrc has enough data (enough-data signal).
 */
static gboolean push_data (CustomData *data) {
  GstBuffer *buffer;
  GstFlowReturn ret;
  int i;
  GstMapInfo map;
  gint16 *raw;
  gint num_samples = CHUNK_SIZE / 2; /* Because each sample is 16 bits */
  gfloat freq;
 
  /* Create a new empty buffer */
  buffer = gst_buffer_new_and_alloc (CHUNK_SIZE);
 
  /* Set its timestamp and duration */
  // gst_util_uint64_scale() 是一个实用函数，可以缩放（乘除）可能很大的数字，而不必担心溢出。
  // 我们使用 CustomData.num_samples 变量计算到目前为止生成的样本数量，因此我们可以使用 GstBuffer中的
  // GST_BUFFER_TIMESTAMP 宏为该缓冲区添加时间戳。

  GST_BUFFER_TIMESTAMP (buffer) = gst_util_uint64_scale (data->num_samples, GST_SECOND, SAMPLE_RATE);
  
  //由于我们正在生成相同大小的缓冲区，它们的持续时间是相同的，通过使用 GstBuffer 中的 GST_BUFFER_DURATION 设置的。
  
  GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale (num_samples, GST_SECOND, SAMPLE_RATE);
  
  //可以使用 GstBuffer 中的 GST_BUFFER_DATA 访问缓冲区的字节（注意不要写到缓冲区的末尾：你分配了它，所以你知道它的大小）
 
  /* Generate some psychodelic waveforms */
  gst_buffer_map (buffer, &map, GST_MAP_WRITE); //用buffer中所有合并内存块的 GstMapInfo 填充map。
  raw = (gint16 *)map.data;
  data->c += data->d;
  data->d -= data->c / 1000;
  freq = 1100 + 1000 * data->d;
  for (i = 0; i < num_samples; i++) {
    data->a += data->b;
    data->b -= data->a / freq;
    raw[i] = (gint16)(500 * data->a);
  }
  
  //释放之前用 gst_buffer_map 映射的内存
  gst_buffer_unmap (buffer, &map);
  data->num_samples += num_samples;
 
  /* Push the buffer into the appsrc */
  g_signal_emit_by_name (data->app_source, "push-buffer", buffer, &ret);
 
  /* Free the buffer now that we are done with it */
  gst_buffer_unref (buffer);
 
  if (ret != GST_FLOW_OK) {
    /* We got some error, stop sending data */
    return FALSE;
  }
 
  return TRUE;
}
 
/* This signal callback triggers when appsrc needs data. Here, we add an idle handler
 * to the mainloop to start pushing data into the appsrc */
static void start_feed (GstElement *source, guint size, CustomData *data) {
  if (data->sourceid == 0) {
    g_print ("Start feeding\n");
    /*
     * 首先function start_feed被call是在appsrc内部队列处于不饱和状态（或已经消耗完了数据）。
     * 此刻我们需要做的是：
     * 使用g_idle_add()函数注册个GLib idle function. GLib idle function 去给appsrc填充数据
     * 直到appsrc内部数据队列再次处于饱和状态。
     * GLib idle function当 GLib 处于“空闲”状态时，即没有更高优先级的任务要执行时，GLib 将从其主循
     * 环调用的方法。显然，它需要一个 GLib GMainLoop 来实例化和运行。特别是，不需要使用 GLib 从主线程
     * 将缓冲区送入 appsrc，并且您不需要使用 need-data 和 enough-data 信号与 appsrc 同步（尽管据称
     * 这是最方便的）。
     * 我们记下 g_idle_add() 返回的 sourceid，因此我们可以稍后禁用它。
    */
    data->sourceid = g_idle_add ((GSourceFunc) push_data, data);
  }
}
 
/* This callback triggers when appsrc has enough data and we can stop sending.
 * We remove the idle handler from the mainloop */
static void stop_feed (GstElement *source, CustomData *data) {
  if (data->sourceid != 0) {
    g_print ("Stop feeding\n");
    /*
     * 首先function stop_feed 被call是在appsrc内部数据队列饱和状态。
     * 此时我们会停止push 数据。
     * 我们只需要简单的使用g_source_remove()函数来移除idle function（idle function 作为GSource实现）。
    */
 
    g_source_remove (data->sourceid);
    data->sourceid = 0;
  }
}
 
/* The appsink has received a buffer */
static GstFlowReturn new_sample (GstElement *sink, CustomData *data) {
  GstSample *sample;
 
  /* Retrieve the buffer */
  g_signal_emit_by_name (sink, "pull-sample", &sample);
  if (sample) {
    /* The only thing we do in this example is print a * to indicate a received buffer */
    /* 此处可以做更复杂的处理 */
    /* 这是在appsink接收缓冲区时调用的函数 。我们使用pull-sample动作信号来检索缓冲区，然后在屏幕上打印一
     * 些指标。我们可以使用GST_BUFFER_DATA宏检索数据指针，使用GST_BUFFER_SIZEin 中的宏检索数据大小GstBuffer。
     * 请记住，此缓冲区不必与我们在push_data函数中生成的缓冲区相匹配，路径中的任何元素都可能以任何方式更改缓冲区
     * 在本例子中，appsrc和appsink 之间只有tee元素，所以确实不会改变缓冲区的内容。
     */
    g_print ("%s: pull-sample! \n",__FUNCTION__);
    //release sample
    gst_sample_unref (sample);
 
    return GST_FLOW_OK;
  }
 
  return GST_FLOW_ERROR;
}
 
/* This function is called when an error message is posted on the bus */
static void error_cb (GstBus *bus, GstMessage *msg, CustomData *data) {
  GError *err;
  gchar *debug_info;
 
  /* Print error details on the screen */
  gst_message_parse_error (msg, &err, &debug_info);
  g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
  g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
  g_clear_error (&err);
  g_free (debug_info);
 
  g_main_loop_quit (data->main_loop);
}
 
int main(int argc, char *argv[]) {
  CustomData data;
  GstPad *tee_audio_pad, *tee_video_pad, *tee_app_pad;
  GstPad *queue_audio_pad, *queue_video_pad, *queue_app_pad;
  GstAudioInfo info;
  GstCaps *audio_caps;
  GstBus *bus;
 
  /* Initialize custom data structure */
  memset (&data, 0, sizeof (data));
  data.b = 1; /* For waveform generation */
  data.d = 1;
 
  /* Initialize GStreamer */
  gst_init (&argc, &argv);
 
  /* Create the elements */
  data.app_source = gst_element_factory_make ("appsrc", "audio_source");
  data.tee = gst_element_factory_make ("tee", "tee");
  data.audio_queue = gst_element_factory_make ("queue", "audio_queue");
  data.audio_convert1 = gst_element_factory_make ("audioconvert", "audio_convert1");
  data.audio_resample = gst_element_factory_make ("audioresample", "audio_resample");
  data.audio_sink = gst_element_factory_make ("autoaudiosink", "audio_sink");
  data.video_queue = gst_element_factory_make ("queue", "video_queue");
  data.audio_convert2 = gst_element_factory_make ("audioconvert", "audio_convert2");
  data.visual = gst_element_factory_make ("wavescope", "visual");
  data.video_convert = gst_element_factory_make ("videoconvert", "video_convert");
  data.video_sink = gst_element_factory_make ("autovideosink", "video_sink");
  data.app_queue = gst_element_factory_make ("queue", "app_queue");
  data.app_sink = gst_element_factory_make ("appsink", "app_sink");
 
  /* Create the empty pipeline */
  data.pipeline = gst_pipeline_new ("test-pipeline");
 
  if (!data.pipeline || !data.app_source || !data.tee || !data.audio_queue || !data.audio_convert1 ||
      !data.audio_resample || !data.audio_sink || !data.video_queue || !data.audio_convert2 || !data.visual ||
      !data.video_convert || !data.video_sink || !data.app_queue || !data.app_sink) {
    g_printerr ("Not all elements could be created.\n");
    return -1;
  }
 
  /* Configure wavescope */
  g_object_set (data.visual, "shader", 0, "style", 0, NULL);
 
  /* Configure appsrc */
  gst_audio_info_set_format (&info, GST_AUDIO_FORMAT_S16, SAMPLE_RATE, 1, NULL);
  
  /* appsrc 第一个需要被设置的属性是caps.它指定元素将要生产的数据类型。所以，GStreamer检查是否可能与下游元素连接
  (意思是，是否下游元素理解这个数据类型)。这个属性必须是GstCaps 对象，它可以很容易用gst_caps_from_string 从一个
  字符串中构建。
  */
  audio_caps = gst_audio_info_to_caps (&info);
  g_object_set (data.app_source, "caps", audio_caps, "format", GST_FORMAT_TIME, NULL);

  //signal "need-data" 表示源需要更多数据(内部数据队列不足)。在回调中或从另一个线程中，您应该调用 push-buffer 或 end-of-stream。
  g_signal_connect (data.app_source, "need-data", G_CALLBACK (start_feed), &data);

  //signal "enough-data"表示源有足够的数据(内部数据队列饱和)。建议应用程序停止调用 push-buffer 直到再次发出需要数据信号以避免过多的缓冲区排队。
  g_signal_connect (data.app_source, "enough-data", G_CALLBACK (stop_feed), &data);
 
  /* Configure appsink */
  /* 关于appsink 配置，我们连接new-sample信号，这个信号在每次sink收到一个buffer时发出。
   */
  g_object_set (data.app_sink, "emit-signals", TRUE, "caps", audio_caps, NULL);

  // signal "new-sample"表示有新sample可用。此信号从流线程发出，并且仅当“emit-signals”属性为TRUE时才发出，
  // 出于性能原因，默认情况下不是这样。。可以使用“pull-sample”动作信号或gst_app_sink_pull_sample从此信号回
  // 调或任何其他线程检索新样本。
  g_signal_connect (data.app_sink, "new-sample", G_CALLBACK (new_sample), &data);
  gst_caps_unref (audio_caps);
 
  /* Link all elements that can be automatically linked because they have "Always" pads */
  gst_bin_add_many (GST_BIN (data.pipeline), data.app_source, data.tee, data.audio_queue, data.audio_convert1, data.audio_resample,
      data.audio_sink, data.video_queue, data.audio_convert2, data.visual, data.video_convert, data.video_sink, data.app_queue,
      data.app_sink, NULL);
  if (gst_element_link_many (data.app_source, data.tee, NULL) != TRUE ||
      gst_element_link_many (data.audio_queue, data.audio_convert1, data.audio_resample, data.audio_sink, NULL) != TRUE ||
      gst_element_link_many (data.video_queue, data.audio_convert2, data.visual, data.video_convert, data.video_sink, NULL) != TRUE ||
      gst_element_link_many (data.app_queue, data.app_sink, NULL) != TRUE) {
    g_printerr ("Elements could not be linked.\n");
    gst_object_unref (data.pipeline);
    return -1;
  }
 
  /* Manually link the Tee, which has "Request" pads */
 
  GstPadTemplate * tee_src_pad_template = gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS (data.tee), "src_%u");
  tee_audio_pad = gst_element_request_pad(data.tee, tee_src_pad_template, nullptr, nullptr);
 
//  tee_audio_pad = gst_element_request_pad_simple (data.tee, "src_%u");
  g_print ("Obtained request pad %s for audio branch.\n", gst_pad_get_name (tee_audio_pad));
  queue_audio_pad = gst_element_get_static_pad (data.audio_queue, "sink");
//  tee_video_pad = gst_element_request_pad_simple (data.tee, "src_%u");
  tee_video_pad =  gst_element_request_pad(data.tee, tee_src_pad_template, nullptr, nullptr);
  g_print ("Obtained request pad %s for video branch.\n", gst_pad_get_name (tee_video_pad));
  queue_video_pad = gst_element_get_static_pad (data.video_queue, "sink");
//  tee_app_pad = gst_element_request_pad_simple (data.tee, "src_%u");
  tee_app_pad = gst_element_request_pad(data.tee, tee_src_pad_template, nullptr, nullptr);
  g_print ("Obtained request pad %s for app branch.\n", gst_pad_get_name (tee_app_pad));
  queue_app_pad = gst_element_get_static_pad (data.app_queue, "sink");
  if (gst_pad_link (tee_audio_pad, queue_audio_pad) != GST_PAD_LINK_OK ||
      gst_pad_link (tee_video_pad, queue_video_pad) != GST_PAD_LINK_OK ||
      gst_pad_link (tee_app_pad, queue_app_pad) != GST_PAD_LINK_OK) {
    g_printerr ("Tee could not be linked\n");
    gst_object_unref (data.pipeline);
    return -1;
  }
  gst_object_unref (queue_audio_pad);
  gst_object_unref (queue_video_pad);
  gst_object_unref (queue_app_pad);
 
  /* Instruct the bus to emit signals for each received message, and connect to the interesting signals */
  bus = gst_element_get_bus (data.pipeline);
  gst_bus_add_signal_watch (bus);
  g_signal_connect (G_OBJECT (bus), "message::error", (GCallback)error_cb, &data);
  gst_object_unref (bus);
 
  /* Start playing the pipeline */
  gst_element_set_state (data.pipeline, GST_STATE_PLAYING);
 
  /* Create a GLib Main Loop and set it to run */
  data.main_loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (data.main_loop);
 
  /* Release the request pads from the Tee, and unref them */
  gst_element_release_request_pad (data.tee, tee_audio_pad);
  gst_element_release_request_pad (data.tee, tee_video_pad);
  gst_element_release_request_pad (data.tee, tee_app_pad);
  gst_object_unref (tee_audio_pad);
  gst_object_unref (tee_video_pad);
  gst_object_unref (tee_app_pad);
 
  /* Free resources */
  gst_element_set_state (data.pipeline, GST_STATE_NULL);
  gst_object_unref (data.pipeline);
  return 0;
}
