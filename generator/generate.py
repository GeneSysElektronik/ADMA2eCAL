import jinja2
from argparse import ArgumentParser
import xml.etree.ElementTree as ET
import os
import dataclasses

def index_generator():
  i = 2
  while True:
    yield str(i)
    i = i + 1

gen = index_generator()

# Filter functions!

def lower_case(value):
  return value.lower()

def protobuf_datatype(channel):
  if channel["DataType"] == "GSFloat":
    return "GSFloat"
  if channel["DataType"] == "IEEEFloat":
    return "float"
  if channel["DataType"] == "BOOL":
    return "bool"
  if channel["Scale"] != 1:
    return "double"
  if channel["DataType"] == "UINT":
    return "uint64"
  return "sint64"


def cpp_datatype(channel):
  if channel["DataType"] == "GSFloat":
    return "double"
  if channel["DataType"] == "IEEEFloat":
    return "float"
  if  channel["DataType"] == "BOOL":
    return "bool"
  if channel["Scale"] != 1:
    return "double"
  if channel["DataType"] == "UINT":
    return "uint64_t"
  return "int64_t"

def is_signed(channel):
  if channel["DataType"] == "GSFloat":
    return ""
  if channel["DataType"] == "IEEEFloat":
    return ""
  if channel["DataType"]  == "BOOL":
    return "false"
  if channel["DataType"]  == "UINT":
    return "false"
  return "true"

# Data definitions

@dataclasses.dataclass
class DataChannel(object):
  Name : str
  Description : str
  Unit : str
  Min : float
  Max : float
  Scale : float
  Offset : float
  DataType: int
  ByteOffset : int
  BitOffset: int
  LengthBits: int
  ChannelID: int

@dataclasses.dataclass
class FormatHeader(object):
  GeneSysID: str
  DynamicHeaderVersion: str
  FormatID: int
  FormatVersion: str
  FormatName: str
  HeaderSize: int
  DataSize: int

def parse_xml(file):
  tree = ET.parse(file)
  root = tree.getroot()

  data_channels = []
  for packages in root.findall('./MeasurementData/'):
    for data_channel in packages.findall('./Channel'):
      Name = data_channel.attrib['Name']
      ChannelID = int(data_channel.attrib['ChannelID'])
      Description = data_channel.find('./Description').text
      Unit = data_channel.find('./Unit').text
      Min = float(data_channel.find('./Min').text)
      Max = float(data_channel.find('./Max').text)
      Scale = float(data_channel.find('./Scale').text)
      #Offset = float(data_channel.find('./Offset').text)
      Offset = 0
      DataType = data_channel.find('./DataType').text
      ByteOffset = int(data_channel.find('./ByteOffset').text)
      BitOffset = int(data_channel.find('./BitOffset').text)
      LengthBits = int(data_channel.find('./LengthBits').text)
      data = DataChannel(Name=Name, Description=Description, Unit=Unit, Min=Min, Max=Max, Scale=Scale, Offset=Offset, 
                         DataType=DataType, ByteOffset=ByteOffset, BitOffset=BitOffset, LengthBits=LengthBits, ChannelID=ChannelID)
      data_channels.append(data)
  return data_channels

def render_templates(data_channels_net, data_channels_delta, output_folder):
  env = jinja2.Environment(loader = jinja2.FileSystemLoader("./templates"))
  env.filters['lower_case'] = lower_case
  env.filters['protobuf_datatype'] = protobuf_datatype
  env.filters['cpp_datatype'] = cpp_datatype
  env.filters['is_signed'] = is_signed

  data = {
    "data_channels_net" : [dataclasses.asdict(data_channel) for data_channel in data_channels_net],
    "data_channels_delta" : [dataclasses.asdict(data_channel) for data_channel in data_channels_delta]
  }
        
  for file in ['Adma.proto', 
               'fill_adma_proto_object.h',
               'fill_adma_proto_object.cpp']:
    t = env.get_template(file)
    content = t.render(**data)
    with open(os.path.join(output_folder, file), "w") as f:
      f.write(content)


def generate_sources(args):
  data_channels_net = parse_xml(args.input_xml_net)
  data_channels_delta = parse_xml(args.input_xml_delta)
  render_templates(data_channels_net= data_channels_net, data_channels_delta=data_channels_delta, output_folder=args.output_folder)
  return 0

def create_parser():
  parser = ArgumentParser(description="Source file generator")
  parser.set_defaults(func=generate_sources)
  parser.add_argument("--input-xml-net", type=str, help="input xml file for adma net", required=True)
  parser.add_argument("--input-xml-delta", type=str, help="input xml file for adma delta", required=True)
  parser.add_argument("--output-folder", type=str, help="output folder where sources will be generated", required=True)
  return parser

if __name__ == "__main__":
  parser = create_parser()
  args = parser.parse_args()
  exit(args.func(args))