// This is a demo code for using a SSD model to do detection.
// The code is modified from examples/cpp_classification/classification.cpp.
// Usage:
//    ssd_detect [FLAGS] model_file weights_file list_file
//
// where model_file is the .prototxt file defining the network architecture, and
// weights_file is the .caffemodel file containing the network parameters, and
// list_file contains a list of image files with the format as follows:
//    folder/img1.JPEG
//    folder/img2.JPEG
// list_file can also contain a list of video files with the format as follows:
//    folder/video1.mp4
//    folder/video2.mp4
//
#include <caffe/caffe.hpp>
#ifdef USE_OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif  // USE_OPENCV
#include <algorithm>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "caffe/layers/input_layer.hpp" 
#include "caffe/layers/inner_product_layer.hpp" 
#include "caffe/layers/dropout_layer.hpp" 
#include "caffe/layers/conv_layer.hpp" 
#include "caffe/layers/relu_layer.hpp" 

#include "caffe/layers/pooling_layer.hpp" 
#include "caffe/layers/lrn_layer.hpp" 
#include "caffe/layers/softmax_layer.hpp" 
#include "caffe/layers/batch_norm_layer.hpp"
#include "caffe/layers/flatten_layer.hpp"
#include "caffe/layers/prelu_layer.hpp"
#include "caffe/layers/concat_layer.hpp"
#include "caffe/layers/reshape_layer.hpp"
#include "caffe/layers/softmax_layer.hpp"
#include "caffe/layers/deconv_layer.hpp"

#include "caffe/layers/slice_layer.hpp"

#include "caffe/layers/contrastive_loss_layer.hpp"
#include "caffe/layers/absval_layer.hpp"
#include "caffe/sgd_solvers.hpp"
#include "caffe/common.hpp" 
#include "caffe/layers/absval_layer.hpp"
#include "caffe/layers/accuracy_layer.hpp"
#include "caffe/layers/annotated_data_layer.hpp"
#include "caffe/layers/argmax_layer.hpp"
#include "caffe/layers/base_conv_layer.hpp"
#include "caffe/layers/base_data_layer.hpp"
#include "caffe/layers/batch_norm_layer.hpp"
#include "caffe/layers/batch_reindex_layer.hpp"
#include "caffe/layers/bias_layer.hpp"
#include "caffe/layers/bnll_layer.hpp"
#include "caffe/layers/concat_layer.hpp"
#include "caffe/layers/contrastive_loss_layer.hpp"
#include "caffe/layers/conv_layer.hpp"
#include "caffe/layers/crop_layer.hpp"
#include "caffe/layers/cudnn_conv_layer.hpp"
#include "caffe/layers/cudnn_lcn_layer.hpp"
#include "caffe/layers/cudnn_lrn_layer.hpp"
#include "caffe/layers/cudnn_pooling_layer.hpp"
#include "caffe/layers/cudnn_relu_layer.hpp"
#include "caffe/layers/cudnn_sigmoid_layer.hpp"
#include "caffe/layers/cudnn_softmax_layer.hpp"
#include "caffe/layers/cudnn_tanh_layer.hpp"
#include "caffe/layers/data_layer.hpp"
#include "caffe/layers/deconv_layer.hpp"
#include "caffe/layers/detection_evaluate_layer.hpp"
#include "caffe/layers/detection_output_layer.hpp"
#include "caffe/layers/dropout_layer.hpp"
#include "caffe/layers/dummy_data_layer.hpp"
#include "caffe/layers/eltwise_layer.hpp"
#include "caffe/layers/elu_layer.hpp"
#include "caffe/layers/embed_layer.hpp"
#include "caffe/layers/euclidean_loss_layer.hpp"
#include "caffe/layers/exp_layer.hpp"
#include "caffe/layers/filter_layer.hpp"
#include "caffe/layers/flatten_layer.hpp"
#include "caffe/layers/hdf5_data_layer.hpp"
#include "caffe/layers/hdf5_output_layer.hpp"
#include "caffe/layers/hinge_loss_layer.hpp"
#include "caffe/layers/im2col_layer.hpp"
#include "caffe/layers/image_data_layer.hpp"
#include "caffe/layers/infogain_loss_layer.hpp"
#include "caffe/layers/inner_product_layer.hpp"
#include "caffe/layers/input_layer.hpp"
#include "caffe/layers/log_layer.hpp"
#include "caffe/layers/loss_layer.hpp"
#include "caffe/layers/lrn_layer.hpp"
#include "caffe/layers/lstm_layer.hpp"
#include "caffe/layers/memory_data_layer.hpp"
#include "caffe/layers/multibox_loss_layer.hpp"
#include "caffe/layers/multinomial_logistic_loss_layer.hpp"
#include "caffe/layers/mvn_layer.hpp"
#include "caffe/layers/neuron_layer.hpp"
#include "caffe/layers/normalize_layer.hpp"
#include "caffe/layers/parameter_layer.hpp"
#include "caffe/layers/permute_layer.hpp"
#include "caffe/layers/pooling_layer.hpp"
#include "caffe/layers/power_layer.hpp"
#include "caffe/layers/prelu_layer.hpp"
#include "caffe/layers/prior_box_layer.hpp"
#include "caffe/layers/python_layer.hpp"
#include "caffe/layers/recurrent_layer.hpp"
#include "caffe/layers/reduction_layer.hpp"
#include "caffe/layers/relu_layer.hpp"
#include "caffe/layers/reshape_layer.hpp"
#include "caffe/layers/rnn_layer.hpp"
#include "caffe/layers/scale_layer.hpp"
#include "caffe/layers/sigmoid_cross_entropy_loss_layer.hpp"
#include "caffe/layers/sigmoid_layer.hpp"
#include "caffe/layers/silence_layer.hpp"
#include "caffe/layers/slice_layer.hpp"
#include "caffe/layers/smooth_L1_loss_layer.hpp"
#include "caffe/layers/softmax_layer.hpp"
#include "caffe/layers/softmax_loss_layer.hpp"
#include "caffe/layers/split_layer.hpp"
#include "caffe/layers/spp_layer.hpp"
#include "caffe/layers/tanh_layer.hpp"
#include "caffe/layers/threshold_layer.hpp"
#include "caffe/layers/tile_layer.hpp"
#include "caffe/layers/video_data_layer.hpp"
#include "caffe/layers/window_data_layer.hpp"

#ifdef USE_OPENCV
using namespace caffe;  // NOLINT(build/namespaces)

namespace caffe {

	extern INSTANTIATE_CLASS(DetectionOutputLayer);
	REGISTER_LAYER_CLASS(DetectionOutput);
	

	extern INSTANTIATE_CLASS(DataLayer);
	REGISTER_LAYER_CLASS(Data);

	extern INSTANTIATE_CLASS(AbsValLayer);
	REGISTER_LAYER_CLASS(AbsVal);

	extern INSTANTIATE_CLASS(AccuracyLayer);
	REGISTER_LAYER_CLASS(Accuracy);

	extern INSTANTIATE_CLASS(AnnotatedDataLayer);
	REGISTER_LAYER_CLASS(AnnotatedData);


	extern INSTANTIATE_CLASS(NormalizeLayer);
	REGISTER_LAYER_CLASS(Normalize);

	extern INSTANTIATE_CLASS(PermuteLayer);
	REGISTER_LAYER_CLASS(Permute);


	extern INSTANTIATE_CLASS(ArgMaxLayer);
	REGISTER_LAYER_CLASS(ArgMax);

	extern INSTANTIATE_CLASS(PriorBoxLayer);
	REGISTER_LAYER_CLASS(PriorBox);

	extern INSTANTIATE_CLASS(MultiBoxLossLayer);
	REGISTER_LAYER_CLASS(MultiBoxLoss);

	extern INSTANTIATE_CLASS(SmoothL1LossLayer);
	REGISTER_LAYER_CLASS(SmoothL1Loss);

	extern INSTANTIATE_CLASS(BatchNormLayer);
	REGISTER_LAYER_CLASS(BatchNorm);


	extern INSTANTIATE_CLASS(BatchReindexLayer);
	REGISTER_LAYER_CLASS(BatchReindex);

	extern INSTANTIATE_CLASS(BiasLayer);
	REGISTER_LAYER_CLASS(Bias);

	extern INSTANTIATE_CLASS(BNLLLayer);
	REGISTER_LAYER_CLASS(BNLL);


	extern INSTANTIATE_CLASS(InputLayer);
	extern INSTANTIATE_CLASS(ConvolutionLayer);
	REGISTER_LAYER_CLASS(Convolution);
	extern INSTANTIATE_CLASS(InnerProductLayer);
	extern INSTANTIATE_CLASS(DropoutLayer);
	extern INSTANTIATE_CLASS(ReLULayer);
	REGISTER_LAYER_CLASS(ReLU);
	extern INSTANTIATE_CLASS(PoolingLayer);
	REGISTER_LAYER_CLASS(Pooling);
	extern INSTANTIATE_CLASS(LRNLayer);
	//REGISTER_LAYER_CLASS(LRN);
	extern INSTANTIATE_CLASS(SoftmaxLayer);
	//REGISTER_LAYER_CLASS(Softmax);


	extern INSTANTIATE_CLASS(FlattenLayer);

	extern INSTANTIATE_CLASS(ConcatLayer);
	extern INSTANTIATE_CLASS(ReshapeLayer);
	extern INSTANTIATE_CLASS(SoftmaxLayer);
	REGISTER_LAYER_CLASS(Softmax);

	extern INSTANTIATE_CLASS(SliceLayer);
	REGISTER_LAYER_CLASS(Slice);


	extern INSTANTIATE_CLASS(ContrastiveLossLayer);
	REGISTER_LAYER_CLASS(ContrastiveLoss);

	extern INSTANTIATE_CLASS(SoftmaxWithLossLayer);
	REGISTER_LAYER_CLASS(SoftmaxWithLoss);

}

class Detector {
 public:
  Detector(const string& model_file,
           const string& weights_file,
           const string& mean_file,
           const string& mean_value);

  std::vector<vector<float> > Detect(const cv::Mat& img);

 private:
  void SetMean(const string& mean_file, const string& mean_value);

  void WrapInputLayer(std::vector<cv::Mat>* input_channels);

  void Preprocess(const cv::Mat& img,
                  std::vector<cv::Mat>* input_channels);

 private:
  shared_ptr<Net<float> > net_;
  cv::Size input_geometry_;
  int num_channels_;
  cv::Mat mean_;
};

Detector::Detector(const string& model_file,
                   const string& weights_file,
                   const string& mean_file,
                   const string& mean_value) {
#ifdef CPU_ONLY
  Caffe::set_mode(Caffe::CPU);
#else
  Caffe::set_mode(Caffe::GPU);
#endif

  /* Load the network. */
  net_.reset(new Net<float>(model_file, TEST));
  net_->CopyTrainedLayersFrom(weights_file);

  CHECK_EQ(net_->num_inputs(), 1) << "Network should have exactly one input.";
  CHECK_EQ(net_->num_outputs(), 1) << "Network should have exactly one output.";

  Blob<float>* input_layer = net_->input_blobs()[0];
  num_channels_ = input_layer->channels();
  CHECK(num_channels_ == 3 || num_channels_ == 1)
    << "Input layer should have 1 or 3 channels.";
  input_geometry_ = cv::Size(input_layer->width(), input_layer->height());

  /* Load the binaryproto mean file. */
  SetMean(mean_file, mean_value);
}

std::vector<vector<float> > Detector::Detect(const cv::Mat& img) {
  Blob<float>* input_layer = net_->input_blobs()[0];
  input_layer->Reshape(1, num_channels_,
                       input_geometry_.height, input_geometry_.width);
  /* Forward dimension change to all layers. */
  net_->Reshape();

  std::vector<cv::Mat> input_channels;
  WrapInputLayer(&input_channels);

  Preprocess(img, &input_channels);

  net_->Forward();

  /* Copy the output layer to a std::vector */
  Blob<float>* result_blob = net_->output_blobs()[0];
  const float* result = result_blob->cpu_data();
  const int num_det = result_blob->height();
  vector<vector<float> > detections;
  for (int k = 0; k < num_det; ++k) {
    if (result[0] == -1 || result[2] < 0.75) {
      // Skip invalid detection.
      result += 7;
      continue;
    }
    vector<float> detection(result, result + 7);
    detections.push_back(detection);
    result += 7;
  }
  return detections;
}

/* Load the mean file in binaryproto format. */
void Detector::SetMean(const string& mean_file, const string& mean_value) {
  cv::Scalar channel_mean;
  if (!mean_file.empty()) {
    CHECK(mean_value.empty()) <<
      "Cannot specify mean_file and mean_value at the same time";
    BlobProto blob_proto;
    ReadProtoFromBinaryFileOrDie(mean_file.c_str(), &blob_proto);

    /* Convert from BlobProto to Blob<float> */
    Blob<float> mean_blob;
    mean_blob.FromProto(blob_proto);
    CHECK_EQ(mean_blob.channels(), num_channels_)
      << "Number of channels of mean file doesn't match input layer.";

    /* The format of the mean file is planar 32-bit float BGR or grayscale. */
    std::vector<cv::Mat> channels;
    float* data = mean_blob.mutable_cpu_data();
    for (int i = 0; i < num_channels_; ++i) {
      /* Extract an individual channel. */
      cv::Mat channel(mean_blob.height(), mean_blob.width(), CV_32FC1, data);
      channels.push_back(channel);
      data += mean_blob.height() * mean_blob.width();
    }

    /* Merge the separate channels into a single image. */
    cv::Mat mean;
    cv::merge(channels, mean);

    /* Compute the global mean pixel value and create a mean image
     * filled with this value. */
    channel_mean = cv::mean(mean);
    mean_ = cv::Mat(input_geometry_, mean.type(), channel_mean);
  }
  if (!mean_value.empty()) {
    CHECK(mean_file.empty()) <<
      "Cannot specify mean_file and mean_value at the same time";
    stringstream ss(mean_value);
    vector<float> values;
    string item;
    while (getline(ss, item, ',')) {
      float value = std::atof(item.c_str());
      values.push_back(value);
    }
    CHECK(values.size() == 1 || values.size() == num_channels_) <<
      "Specify either 1 mean_value or as many as channels: " << num_channels_;

    std::vector<cv::Mat> channels;
    for (int i = 0; i < num_channels_; ++i) {
      /* Extract an individual channel. */
      cv::Mat channel(input_geometry_.height, input_geometry_.width, CV_32FC1,
          cv::Scalar(values[i]));
      channels.push_back(channel);
    }
    cv::merge(channels, mean_);
  }
}

/* Wrap the input layer of the network in separate cv::Mat objects
 * (one per channel). This way we save one memcpy operation and we
 * don't need to rely on cudaMemcpy2D. The last preprocessing
 * operation will write the separate channels directly to the input
 * layer. */
void Detector::WrapInputLayer(std::vector<cv::Mat>* input_channels) {
  Blob<float>* input_layer = net_->input_blobs()[0];

  int width = input_layer->width();
  int height = input_layer->height();
  float* input_data = input_layer->mutable_cpu_data();
  for (int i = 0; i < input_layer->channels(); ++i) {
    cv::Mat channel(height, width, CV_32FC1, input_data);
    input_channels->push_back(channel);
    input_data += width * height;
  }
}

void Detector::Preprocess(const cv::Mat& img,
                            std::vector<cv::Mat>* input_channels) {
  /* Convert the input image to the input image format of the network. */
  cv::Mat sample;
  if (img.channels() == 3 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGR2GRAY);
  else if (img.channels() == 4 && num_channels_ == 1)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2GRAY);
  else if (img.channels() == 4 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_BGRA2BGR);
  else if (img.channels() == 1 && num_channels_ == 3)
    cv::cvtColor(img, sample, cv::COLOR_GRAY2BGR);
  else
    sample = img;

  cv::Mat sample_resized;
  if (sample.size() != input_geometry_)
    cv::resize(sample, sample_resized, input_geometry_);
  else
    sample_resized = sample;

  cv::Mat sample_float;
  if (num_channels_ == 3)
    sample_resized.convertTo(sample_float, CV_32FC3);
  else
    sample_resized.convertTo(sample_float, CV_32FC1);

  cv::Mat sample_normalized;
  cv::subtract(sample_float, mean_, sample_normalized);

  /* This operation will write the separate BGR planes directly to the
   * input layer of the network because it is wrapped by the cv::Mat
   * objects in input_channels. */
  cv::split(sample_normalized, *input_channels);

  CHECK(reinterpret_cast<float*>(input_channels->at(0).data)
        == net_->input_blobs()[0]->cpu_data())
    << "Input channels are not wrapping the input layer of the network.";
}

DEFINE_string(mean_file, "",
    "The mean file used to subtract from the input image.");
DEFINE_string(mean_value, "104,117,123",
    "If specified, can be one value or can be same as image channels"
    " - would subtract from the corresponding channel). Separated by ','."
    "Either mean_file or mean_value should be provided, not both.");
DEFINE_string(file_type, "image",
    "The file type in the list_file. Currently support image and video.");
DEFINE_string(out_file, "",
    "If provided, store the detection results in the out_file.");
DEFINE_double(confidence_threshold, 0.01,
    "Only store detections with score higher than the threshold.");

//bison 临时加上这些标签信息，并不通用。只能用于PASCAL VOC数据集
std::string g_labels[] = { 
	"background",
	"aeroplane",
	"bicycle",
	"bird",
	"boat",
	"bottle",
	"bus",
	"car",
	"cat",
	"chair",
	"cow",
	"diningtable",
	"dog",
	"horse",
	"motorbike",
	"person",
	"pottedplant",
	"sheep",
	"sofa",
	"train",
	"tvmonitor"
};

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  // Print output to stderr (while still logging)
  FLAGS_alsologtostderr = 1;

#ifndef GFLAGS_GFLAGS_H_
  namespace gflags = google;
#endif

  gflags::SetUsageMessage("Do detection using SSD mode.\n"
        "Usage:\n"
        "    ssd_detect [FLAGS] model_file weights_file list_file\n");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  if (argc < 4) {
    gflags::ShowUsageWithFlagsRestrict(argv[0], "examples/ssd/ssd_detect");
    return 1;
  }

  const string& model_file = argv[1];
  const string& weights_file = argv[2];
  const string& mean_file = FLAGS_mean_file;
  const string& mean_value = FLAGS_mean_value;
  const string& file_type = FLAGS_file_type;
  const string& out_file = FLAGS_out_file;
  const float confidence_threshold = FLAGS_confidence_threshold;

  printf("mean_file:%s,mean_val:%s, threshold:%.2f\n", mean_file, mean_value, confidence_threshold);

  // Initialize the network.
  Detector detector(model_file, weights_file, mean_file, mean_value);

  // Set the output mode.
  std::streambuf* buf = std::cout.rdbuf();
  std::ofstream outfile;
  if (!out_file.empty()) {
    outfile.open(out_file.c_str());
    if (outfile.good()) {
      buf = outfile.rdbuf();
    }
  }
  std::ostream out(buf);

  // Process image one by one.
  std::ifstream infile(argv[3]);
  std::string file;
  while (infile >> file) {
    if (file_type == "image") {
      cv::Mat img = cv::imread(file, -1);
      CHECK(!img.empty()) << "Unable to decode image " << file;
      std::vector<vector<float> > detections = detector.Detect(img);

      /* Print the detection results. */
      for (int i = 0; i < detections.size(); ++i) {
        const vector<float>& d = detections[i];
        // Detection format: [image_id, label, score, xmin, ymin, xmax, ymax].
        CHECK_EQ(d.size(), 7);
        const float score = d[2];
        if (score >= confidence_threshold) {
			int label = static_cast<int>(d[1]);
			string label_str = g_labels[label];
          out << file << " ";
          out << label_str << " "; // label
          out << score << " "; //score 
          out << static_cast<int>(d[3] * img.cols) << " "; //xmin
          out << static_cast<int>(d[4] * img.rows) << " "; //ymin
          out << static_cast<int>(d[5] * img.cols) << " ";
          out << static_cast<int>(d[6] * img.rows) << std::endl;
        }
      }
    } else if (file_type == "video") {
      cv::VideoCapture cap(file);
      if (!cap.isOpened()) {
        LOG(FATAL) << "Failed to open video: " << file;
      }
      cv::Mat img;
      int frame_count = 0;
      while (true) {
        bool success = cap.read(img);
        if (!success) {
          LOG(INFO) << "Process " << frame_count << " frames from " << file;
          break;
        }
        CHECK(!img.empty()) << "Error when read frame";
        std::vector<vector<float> > detections = detector.Detect(img);

        /* Print the detection results. */
        for (int i = 0; i < detections.size(); ++i) {
          const vector<float>& d = detections[i];
          // Detection format: [image_id, label, score, xmin, ymin, xmax, ymax].
          CHECK_EQ(d.size(), 7);
          const float score = d[2];
          if (score >= confidence_threshold) {
            out << file << "_";
            out << std::setfill('0') << std::setw(6) << frame_count << " ";
            out << static_cast<int>(d[1]) << " ";
            out << score << " ";
            out << static_cast<int>(d[3] * img.cols) << " ";
            out << static_cast<int>(d[4] * img.rows) << " ";
            out << static_cast<int>(d[5] * img.cols) << " ";
            out << static_cast<int>(d[6] * img.rows) << std::endl;
          }
        }
        ++frame_count;
      }
      if (cap.isOpened()) {
        cap.release();
      }
    } else {
      LOG(FATAL) << "Unknown file_type: " << file_type;
    }
  }
  return 0;
}
#else
int main(int argc, char** argv) {
  LOG(FATAL) << "This example requires OpenCV; compile with USE_OPENCV.";
}
#endif  // USE_OPENCV
