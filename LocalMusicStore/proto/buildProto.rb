protos = Dir.glob("*.proto")
impl = Dir.glob("impl/*.pb*")

protoChanges = protos.map &File.method(:mtime)
implChanges = impl.map &File.method(:mtime)

latestProto = protoChanges.max
latestImpl = implChanges.max

latestProto = Time.at(0) if latestProto == nil
latestImpl = Time.at(0) if latestImpl == nil

if latestProto >= latestImpl
  puts "Generating Protos..."
  DEBUG_PROTOC = "../../protobuf/vs/Debug/protoc.exe"
  RELEASE_PROTOC = "../../protobuf/vs/Release/protoc.exe"
  protoc = DEBUG_PROTOC if File.exist?(DEBUG_PROTOC)
  protoc = RELEASE_PROTOC if File.exist?(RELEASE_PROTOC)
  protos.each do |protoPath|
    `#{protoc} --cpp_out=./impl/ #{protoPath}`
  end
  puts "Proto Generation Complete."
end