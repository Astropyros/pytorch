#pragma once
#include <ATen/Config.h>
#include <ATen/core/Tensor.h>
#include <ATen/ops/empty.h>
#include <torch/library.h>
#if AT_KLEIDIAI_ENABLED()

namespace at::native::kleidiai {

template <typename T>
void kai_pack_rhs_groupwise_int4(
    T& kernel,
    const Tensor& weight_packed,
    const Tensor& weight,
    const Tensor& scales,
    const Tensor& bias,
    const int64_t n,
    const int64_t k,
    const int64_t bl,
    const int64_t rhs_stride,
    const int64_t scale_stride) {
  // kai supports 32 block size only
  const auto& ukernel = kernel.ukernel;
  const size_t nr = ukernel.get_nr();
  const size_t kr = ukernel.get_kr();
  const size_t sr = ukernel.get_sr();
  auto weight_packed_data =
      reinterpret_cast<uint8_t*>(weight_packed.data_ptr());
  const auto weight_data = weight.data_ptr<uint8_t>();
  auto scales_data = scales.const_data_ptr();

  if (weight_data == nullptr) {
    AT_ERROR("kai_pack_rhs_channelwise_int4: Weight data pointer is null");
  }

  if (scales_data == nullptr) {
    AT_ERROR("kai_pack_rhs_channelwise_int4: Scales data pointer is null");
  }

  float* bias_ptr = NULL;
  if (bias.numel())
    bias_ptr = bias.data_ptr<float>();

  auto& params = kernel.rhs_pack_params;

  kernel.kai_run_rhs_pack(
      1,
      n,
      k,
      nr,
      kr,
      sr,
      bl,
      (const uint8_t*)(weight_data),
      rhs_stride,
      bias_ptr,
      scales_data,
      scale_stride,
      weight_packed_data,
      0,
      &params);
}

template <typename T>
void kai_pack_rhs_channelwise_int4(
    T& kernel,
    const Tensor& weight_packed,
    const Tensor& weight,
    const Tensor& scales,
    const Tensor& bias,
    const int64_t n,
    const int64_t k) {
  const auto& ukernel = kernel.ukernel;
  const size_t nr = ukernel.get_nr();
  const size_t kr = ukernel.get_kr();
  const size_t sr = ukernel.get_sr();
  auto weight_packed_data =
      reinterpret_cast<uint8_t*>(weight_packed.data_ptr());
  const auto weight_data = weight.data_ptr<uint8_t>();
  const auto scales_data = scales.data_ptr<float>();

  if (weight_data == nullptr) {
    AT_ERROR("kai_pack_rhs_channelwise_int4: Weight data pointer is null");
  }

  if (scales_data == nullptr) {
    AT_ERROR("kai_pack_rhs_channelwise_int4: Scales data pointer is null");
  }

  float* bias_ptr = NULL;
  if (bias.numel())
    bias_ptr = bias.data_ptr<float>();

  auto& params = kernel.rhs_pack_params;

  kernel.kai_run_rhs_pack(
      1,
      n,
      k,
      nr,
      kr,
      sr,
      (const uint8_t*)(weight_data),
      (const float*)(bias_ptr),
      (const float*)(scales_data),
      weight_packed_data,
      0,
      &params);
}

} // namespace at::native::kleidiai

#endif
