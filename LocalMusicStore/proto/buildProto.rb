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
  protos.each do |protoPath|
    `../../bin/protoc/protoc.exe --cpp_out=./impl/ #{protoPath}`
  end
  puts "Proto Generation Complete."
end