#include <fill_adma_proto_object.h>
#include <get_value.h>

void fill_adma_proto_object(const std::vector<char>& input, pb::ADMA::ADMA_net& adma)
{
  // double get_value(const std::vector<char>& input, int byte_offset, int bit_offset, int length_bits, float scale, float offset, int data_type, double min, double max);
  {% for data in data_channels_net %}
  {%- if data["DataType"] == "IEEEFloat" %}
  adma.set_{{ data.Name | lower_case }}(get_float(input, {{ data.ByteOffset }}, {{ data.BitOffset }}, {{ data.LengthBits }}));
  {%- elif data["DataType"] == "GSFloat" %}
  {
    auto {{ data.Name | lower_case }} = adma.mutable_{{ data.Name | lower_case }}();
    int32_t integer_part = get_value<int32_t>(input, {{ data.ByteOffset }}, {{ data.BitOffset }}, 32, 1, 0, true, {{ data.Min }}, {{ data.Max }});
    float floating_part = get_float(input, {{ data.ByteOffset }}+4, {{ data.BitOffset }}, 32);
    {{ data.Name | lower_case }}->set_integer(integer_part);
    {{ data.Name | lower_case }}->set_decimal(floating_part);
    {{ data.Name | lower_case }}->set_interpreted(integer_part * 0.001 + floating_part * 0.001);  
  }
  {%- else %}
  adma.set_{{ data.Name | lower_case }}(get_value<{{ data | cpp_datatype }}>(input, {{ data.ByteOffset }}, {{ data.BitOffset }}, {{ data.LengthBits }}, {{ data.Scale }}, {{ data.Offset }}, {{ data | is_signed }}, {{ data.Min }}, {{ data.Max }}));
  {%- endif %}
  {%- endfor %}
}

void fill_adma_proto_object(const std::vector<char>& input, pb::ADMA::ADMA_delta& adma)
{
  // double get_value(const std::vector<char>& input, int byte_offset, int bit_offset, int length_bits, float scale, float offset, int data_type, double min, double max);
  {% for data in data_channels_delta %}
  {%- if data["DataType"] == "IEEEFloat" %}
  adma.set_{{ data.Name | lower_case }}(get_float(input, {{ data.ByteOffset }}, {{ data.BitOffset }}, {{ data.LengthBits }}));
  {%- elif data["DataType"] == "GSFloat" %}
  {
    auto {{ data.Name | lower_case }} = adma.mutable_{{ data.Name | lower_case }}();
    int32_t integer_part = get_value<int32_t>(input, {{ data.ByteOffset }}, {{ data.BitOffset }}, 32, 1, 0, true, {{ data.Min }}, {{ data.Max }});
    float floating_part = get_float(input, {{ data.ByteOffset }}+4, {{ data.BitOffset }}, 32);
    {{ data.Name | lower_case }}->set_integer(integer_part);
    {{ data.Name | lower_case }}->set_decimal(floating_part);
    {{ data.Name | lower_case }}->set_interpreted(integer_part * 0.001 + floating_part * 0.001); 
  }
  {%- else %}
  adma.set_{{ data.Name | lower_case }}(get_value<{{ data | cpp_datatype }}>(input, {{ data.ByteOffset }}, {{ data.BitOffset }}, {{ data.LengthBits }}, {{ data.Scale }}, {{ data.Offset }}, {{ data | is_signed }}, {{ data.Min }}, {{ data.Max }}));
  {%- endif %}
  {%- endfor %}
}

