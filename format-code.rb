allCodeFiles = Dir.glob("**/*.{cpp,hpp,c,h,}").select { |path|
  !path.downcase.include?("boost");
}
allCodeFiles.select! { |path|
  !path.downcase.include?("protobuf");
}
allCodeFiles.select! { |path|
  !path.downcase.include?(".g.");
}

allCodeFiles.each { |path|
  system("clang-format -i #{path}");
}