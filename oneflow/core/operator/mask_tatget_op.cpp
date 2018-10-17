#include "oneflow/core/operator/mask_target_op.h"

namespace oneflow {

void MaskTargetOp::InitFromOpConf() {
  CHECK_EQ(this->device_type(), DeviceType::kCPU);
  CHECK(op_conf().has_mask_target_conf());
  // Enroll input
  EnrollInputBn("sample_rois", false);
  EnrollInputBn("sample_labels", false);
  EnrollInputBn("seg_polys", false);
  EnrollInputBn("seg_cls", false);
  // Enroll output
  EnrollOutputBn("mask_rois", false);
  EnrollOutputBn("masks", false);
  // Enroll data tmp
  EnrollDataTmpBn("rois_index");
  EnrollDataTmpBn("max_overlaps");
  EnrollDataTmpBn("max_overlaps_mask_boxes_index");
}

const PbMessage& MaskTargetOp::GetCustomizedConf() const { return op_conf().mask_target_conf(); }

void MaskTargetOp::InferBlobDescs(std::function<BlobDesc*(const std::string&)> GetBlobDesc4BnInOp,
                                  const ParallelContext* parallel_ctx) const {
  const MaskTargetOpConf& conf = op_conf().mask_target_conf();
  // TODO: Check conf
  // input: sample_rois (R, 5) T
  const BlobDesc* sample_rois_blob_desc = GetBlobDesc4BnInOp("sample_rois");
  // input: sample_labels (R) int32_t
  const BlobDesc* sample_labels_blob_desc = GetBlobDesc4BnInOp("sample_labels");
  // input: seg_polys (n,g,b) float
  const BlobDesc* seg_polys_blob_desc = GetBlobDesc4BnInOp("seg_polys");
  // input: seg_cls (n,g) int32_t
  const BlobDesc* seg_cls_blob_desc = GetBlobDesc4BnInOp("seg_cls");

  CHECK_EQ(seg_polys_blob_desc->shape().At(1), seg_cls_blob_desc->shape().At(1));
  int64_t R = sample_rois_blob_desc->shape().At(0);
  int64_t N = seg_polys_blob_desc->shape().At(0);
  int64_t roi_per_img = conf.num_rois_per_image();
  int64_t class_num = conf.num_classes();
  int64_t max_gt_boxes_num = conf.max_gt_boxes_num();
  int64_t M = conf.resolution();

  DataType data_type = sample_rois_blob_desc->data_type();
  // output: mask_rois (fgR, 5) T
  BlobDesc* mask_rois_blob_desc = GetBlobDesc4BnInOp("mask_rois");
  mask_rois_blob_desc->mut_shape() = Shape({N * roi_per_img, 5});
  mask_rois_blob_desc->set_data_type(data_type);
  mask_rois_blob_desc->set_has_dim0_valid_num_field(true);
  mask_rois_blob_desc->mut_dim0_inner_shape() = Shape({1, N * roi_per_img});
  // output: masks (n * fg_num, class_num , M , M) T
  BlobDesc* masks_blob_desc = GetBlobDesc4BnInOp("masks");
  masks_blob_desc->mut_shape() = Shape({N * roi_per_img, class_num, M, M});
  masks_blob_desc->set_data_type(data_type);
  masks_blob_desc->set_has_dim0_valid_num_field(true);
  masks_blob_desc->mut_dim0_inner_shape() = Shape({1, N * roi_per_img});
  // data tmp: roi_index (rois_num) int32_t
  BlobDesc* boxes_index_blob_desc = GetBlobDesc4BnInOp("rois_index");
  boxes_index_blob_desc->mut_shape() = Shape({R});
  boxes_index_blob_desc->set_data_type(DataType::kInt32);
  // data tmp: max_overlaps (rois_num) float
  BlobDesc* max_overlaps_blob_desc = GetBlobDesc4BnInOp("max_overlaps");
  max_overlaps_blob_desc->mut_shape() = Shape({R});
  max_overlaps_blob_desc->set_data_type(DataType::kFloat);
  // data tmp: max_overlaps_mask_boxes_index (rois_num) int32_t
  BlobDesc* max_overlaps_mask_boxes_index_blob_desc =
      GetBlobDesc4BnInOp("max_overlaps_mask_boxes_index");
  max_overlaps_mask_boxes_index_blob_desc->mut_shape() = Shape({R});
  max_overlaps_mask_boxes_index_blob_desc->set_data_type(DataType::kInt32);
}

REGISTER_OP(OperatorConf::kMaskTargetConf, MaskTargetOp);
}  // namespace oneflow