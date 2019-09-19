#include "oneflow/core/operator/operator.h"

namespace oneflow {

class PieceSliceOp final : public Operator {
 public:
  OF_DISALLOW_COPY_AND_MOVE(PieceSliceOp);
  PieceSliceOp() = default;
  ~PieceSliceOp() = default;

  void InitFromOpConf() {
    CHECK(op_conf().has_piece_slice_conf());
    EnrollInputBn("in");
    EnrollRepeatedOutputBn("out");
  }

  const PbMessage& GetCustomizedConf() const override { return op_conf().piece_slice_conf(); }

  Maybe<void> InferBlobDescs(std::function<BlobDesc*(const std::string&)> GetBlobDesc4BnInOp,
                             const ParallelContext* parallel_ctx) const override {
    const BlobDesc* in = GetBlobDesc4BnInOp(SoleIbn());
    const int32_t out_size = op_conf().piece_slice_conf().out_size();
    CHECK(in->has_lod());
    FOR_RANGE(int32_t, i, 0, out_size) {
      BlobDesc* out_i = GetBlobDesc4BnInOp(output_bns().Get(i));
      out_i->mut_static_shape() = Shape(std::vector<int64_t>(
          in->static_shape().dim_vec().begin() + 1, in->static_shape().dim_vec().end()));
      out_i->set_data_type(in->data_type());
      out_i->set_has_lod(false);
    }
  }

 private:
  Maybe<void> InferBatchAxis(
      const std::function<const BlobDesc&(const std::string&)>& LogicalBlobDesc4Ibn,
      std::function<OptInt64*(const std::string&)> BatchAxis4BnInOp) const {
    // TODO
  }
  Maybe<void> GetSbpSignatures(
      const std::function<Maybe<const BlobDesc*>(const std::string&)>& LogicalBlobDesc4Ibn,
      SbpSignatureList* sbp_sig_list) const {
    // TODO
  }
};

}  // namespace oneflow
