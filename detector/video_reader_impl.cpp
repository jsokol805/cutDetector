#include <video_reader_impl.hpp>

bool VideoReaderImpl::openFile(std::string filename) {
  if (videoFile_.open(filename)) {
    filename_ = filename;
    return true;
  }
  return false;
}

void VideoReaderImpl::closeFile() {
  videoFile_.release();
}

std::string VideoReaderImpl::getCurrentFilename() {
  return filename_;
}

bool VideoReaderImpl::isOpen() {
  return videoFile_.isOpened();
}

bool VideoReaderImpl::getFrame(int frameIndex, cv::Mat& result) {
  if (!isOpen() || frameIndex < 0 || frameIndex > getTotalFrameCount())
    return false;
  // We can't read frames backwards. Reopen file to start from beginning.
  if (frameIndex < getCurrentFrameIndex()) {
    openFile(filename_);
  }
  // Skip frames till we reach desired frameIndex.
  while (getCurrentFrameIndex() != frameIndex) {
    videoFile_.grab();
  }
  videoFile_.retrieve(result);
  OnCurrentVideoFrameChanged(result, frameIndex);
  return true;
}

int VideoReaderImpl::getTotalFrameCount() {
  if (isOpen())
    return static_cast<int>(videoFile_.get(CV_CAP_PROP_FRAME_COUNT));
  return 0;
}

int VideoReaderImpl::getCurrentFrameIndex() {
  if (isOpen())
    return static_cast<int>(videoFile_.get(CV_CAP_PROP_POS_FRAMES));
  return 0;
}

int VideoReaderImpl::getFrameHeight() {
  return isOpen() ? static_cast<int>(videoFile_.get(CV_CAP_PROP_FRAME_HEIGHT))
                  : 0;
}

int VideoReaderImpl::getFrameWidth() {
  return isOpen() ? static_cast<int>(videoFile_.get(CV_CAP_PROP_FRAME_WIDTH))
                  : 0;
}

void VideoReaderImpl::OnCurrentVideoFrameChanged(Mat& frame, int index) {
  for (auto observer : observers_) {
    observer->onCurrentFrameChanged(frame, index);
  }
}

void VideoReaderImpl::RegisterObserver(VideoReader::Observer* observer) {
  observers_.push_back(observer);
}

void VideoReaderImpl::UnregisterObserver(VideoReader::Observer* observer) {
  observers_.erase(std::remove(observers_.begin(), observers_.end(), observer),
                   observers_.end());
}
