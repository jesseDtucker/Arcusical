
IGNORE = [
  "boost",
  "protobuf",
  ".g."
];
EXTENSIONS = [
  "cpp",
  "hpp",
  "c",
  "h"
];
updateAll = false;

ARGV.each { |arg|
  if (arg.downcase == "-all") then
    updateAll = true
  end
}

allCodeFiles = [];
if (updateAll) then
  allCodeFiles = Dir.glob("**/*.{cpp,hpp,c,h}")
else
  diff = `git diff --staged`
  fileLines = diff.lines.select { |line|
    line.start_with?("+++ b/");
  }
  fileLines.map! { |line|
    line.slice!("+++ b/")
    line
  }
  fileLines.select! { |file|
    EXTENSIONS.any? { |ext|
      file.downcase.strip.end_with?(ext)
    }
  }
  allCodeFiles = fileLines
end
IGNORE.each { |ignore|
  allCodeFiles.select! { |path|
    !path.downcase.include?(ignore);
  }
}

allCodeFiles.each { |path|
  system("clang-format -i #{path}");
}