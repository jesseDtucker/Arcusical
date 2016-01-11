IGNORE = [
  "boost",
  "protobuf",
  ".g.",
  "AlacCodec",
  ".pb."
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
    !path.downcase.include?(ignore.downcase);
  }
}

def getIncludes(contents)
  contents = contents.select { |line|
    line.include?("#include")
  }
  contents.map { |line|
    line.match(/#include.*/)[0] + "\n"
  }
end

def removeLeadingEmptyLines(stuff)
  hasFoundNonWhiteSpace = false
  stuff.select { |line|
    keepLine = hasFoundNonWhiteSpace
    if (!hasFoundNonWhiteSpace)
      keepLine = line.rstrip.size > 0
      hasFoundNonWhiteSpace = keepLine
    end
    keepLine
  }
end

def stripExtraLines(stuff)
  stuff = removeLeadingEmptyLines(stuff.reverse)
  stuff = removeLeadingEmptyLines(stuff.reverse)
end

def getEverythingElse(contents)
  everythingElse = contents.select { |line|
    !line.include?("#include") && !line.include?("#pragma once")
  }
  stripExtraLines(everythingElse)
end

# cleanup the include whitespace so clang sorts properly
allCodeFiles.each { |path|
  path.chomp!
  contents = []
  File.open(path, "r") { |f|
    contents = f.readlines
    includes = getIncludes(contents)
    everythingElse = getEverythingElse(contents)
    contents = includes + ["\n"] + everythingElse
  }
  #File.open(path, "w") { |f|
    #f.write(contents.join)
  #}
}

allCodeFiles.each { |path|
  system("clang-format -i #{path}");
}

def isLibraryHeader?(importLine)
  return importLine.start_with?("#include <") || importLine.start_with?("#include \"boost")
end

def cleanupHeaderFile(contents)
  contents = stripExtraLines(contents)
  removedHeaderGuard = false
  withoutHeaderGuards = contents.select { |line|
    line = line.strip
    hasHeaderGuard = ((line.start_with?("#ifndef") || line.start_with?("#define")) &&
                      (line.end_with?("_HPP") || line.end_with?("_H")))
    if (!removedHeaderGuard)
      removedHeaderGuard = hasHeaderGuard
    end
    !(line.include?("#pragma once") || hasHeaderGuard)
  }
  if(removedHeaderGuard && withoutHeaderGuards.size > 0 && withoutHeaderGuards.reverse.first.include?("#endif"))
    withoutHeaderGuards = withoutHeaderGuards.reverse[1..withoutHeaderGuards.size].reverse
  end

  return stripExtraLines(["#pragma once\n\n"] + withoutHeaderGuards)
end

def headerFileName(path)
  fileName = File.basename(path, ".cpp")
  fileName + ".hpp"
end

def cleanupImplFile(contents, path)
  headerFile = headerFileName(path)
  #pch always comes first
  pch = contents.select { |line|
    line.include?("#include \"pch.h\"")
  }
  associatedHeader = contents.select { |line|
    line.include?(headerFile) && line.include?("#include")
  }
  everythingElse = contents.select { |line|
    !pch.include?(line) && !associatedHeader.include?(line)
  }

  stripExtraLines(pch) + stripExtraLines(associatedHeader) + stripExtraLines(everythingElse)
end

# now do some seperate of the includes to make sure there is some whitespace where desired
allCodeFiles.each { |path|
  path.chomp!
  contents = []
  File.open(path, "r") { |f|
    contents = f.readlines
    if(path.end_with?("cpp"))
      contents = cleanupImplFile(contents, path)
    end
    includes = getIncludes(contents)
    everythingElse = getEverythingElse(contents)
    includesWithLineBreaks = [];
    prevLine = "";
    headerFile = headerFileName(path)
    includes.each { |line|
      if (isLibraryHeader?(prevLine) && !isLibraryHeader?(line))
        includesWithLineBreaks.push("\n")
      end
      includesWithLineBreaks.push(line)
      if ((line.include?("pch.h") || line.include?(headerFile)) && line.include?("#include"))
        includesWithLineBreaks.push("\n")
      end
      prevLine = line
    }
    contents = stripExtraLines(includesWithLineBreaks) + ["\n"] + stripExtraLines(everythingElse)
    if(path.end_with?("hpp") || path.end_with?("h"))
      contents = cleanupHeaderFile(contents)
    end
    stripExtraLines(contents) + ["\n"]
  }
  File.open(path, "w") { |f|
    f.write(contents.join)
  }
}