#!/bin/bash

## Enable extended pattern matching features if not already set
shopt -q extglob; extglob_set=$?
((extglob_set)) && shopt -s extglob

PKG_CONFIG_PATH=$BUILD_PATH/lib/pkgconfig

if which tput >/dev/null 2>&1; then
  ncolors=$(tput colors)
  if test -n "$ncolors" && test "$ncolors" -ge 8; then
    bold=$(tput bold)
    blue=$(tput setaf 4)
    orange=$(tput setaf 3)
    green=$(tput setaf 2)
    red=$(tput setaf 1)
    reset=$(tput sgr0)
  fi
  ncols=72
fi

doPrintStatus() {
  local name="$1 "
  local color="$2"
  local status="$3"
  local pad
  pad=$(printf '%0.1s' "."{1..72})
  local padlen=$((ncols-${#name}-${#status}-3))
  printf '%s%*.*s [%s]\n' "${bold}$name${reset}" 0 "$padlen" "$pad" "${color}${status}${reset}"
}

doPrintProgress() {
  [[ ${1} =~ ^[a-zA-Z] ]] && echo "├ $*..." || echo -e "$*..."
}

doPrintMessage() {
  echo -e "├ $*"
}

doFailure() {
  doPrintProgress "Fatal error"
  exit 3
}

fileInstalled() {
  local file name
  case $1 in
    /* )
      file="$1" ;;
    *.pc )
      file="lib/pkgconfig/$1" ;;
    *.a|*.la|*.lib )
      file="lib/$1" ;;
    *.h|*.hpp|*.c )
      file="include/$1" ;;
    * )
      file="$1" ;;
  esac

  name=${file#*/}
  name=${name%.*}

  [[ ${file::1} != "/" ]] && file="$BUILD_PATH/$file"
  
  if [ -f "$file" ]; then
    doPrintStatus "├ $name" "${green}" "Found"
  else
    doPrintStatus "├ $name" "${red}" "Not found"
  fi

  test -e "$file"
}

getArchive() {
  local url="${1}"
  [ "$#" -eq 2 ] && shift
  local file_name="${1##*/}"
  local archive_name="${file_name%%.*}"
  local archive_ext="${file_name#*.}"
  local archive_path=

  cd "$EXTERNAL_PATH"
  if [ ! -f $file_name ]; then
    doPrintProgress "Downloading $file_name"
    wget -q -O $file_name $url >/dev/null 2>&1 || return 0
  fi

  if [ "$archive_ext" == "zip" ]; then
    archive_path="$archive_name"
    [ ! -d $archive_path ] && unzip "$file_name"
  else 
    archive_path=$( tar -tf "$file_name" | sed 's!\([^\/]*\)\/.*!\1!' | head -n1 - )
    [ ! -d $archive_path ] && tar -xf "$file_name"
  fi

  cd "$archive_path"
}

getGit() {
  local url="${1}"
  local repo_name="${url##*/}"
  repo_name="${repo_name%%.*}"

  if [ ! -d $repo_name ]; then
    doPrintProgress "Cloning from repo"
    git clone "$url"  >/dev/null 2>&1 || doFailure
    cd "$repo_name"
  else
    cd "$repo_name"
  fi
}

getMercurial() {
  local url="${1}"
  local repo_name="${url##*/}"

  if [ -d $repo_name ]; then
    rm -rf "$repo_name"
  fi

  doPrintProgress "Cloning from repo"
  hg clone "$url"  >/dev/null 2>&1 || doFailure
  cd "$repo_name"
}

makeClean() {
  local name="${1}"
  doPrintProgress "Cleaning $name"
  cd "${EXTERNAL_PATH}/$name"

  make clean >/dev/null 2>&1 || doFailure
}

configureBuild() {
  local current_dir="$( pwd -P )"
  current_dir="${current_dir##*/}"

  doPrintProgress "Configuring $current_dir"
  PATH="$BUILD_PATH/bin:$PATH" ./configure --prefix="$BUILD_PATH" "$@" >/dev/null 2>&1 || doFailure

  doPrintProgress "Building "
  PATH="$BUILD_PATH/bin:$PATH" make >/dev/null 2>&1 || doFailure

  make install >/dev/null 2>&1 || doFailure
}

reconfigureBuild() {
  ./autogen.sh >/dev/null 2>&1 || doFailure
  configureBuild "$@" 
}

cmakeBuild() {
  local current_dir="$( pwd -P )"
  current_dir="${current_dir##*/}"

  doPrintProgress "Configuring $current_dir"
  PATH="$BUILD_PATH/bin:$PATH" cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="$BUILD_PATH" "$@" >/dev/null 2>&1 || doFailure

  doPrintProgress "Building "
  make >/dev/null 2>&1 || doFailure

  make install >/dev/null 2>&1 || doFailure
}

getTools() {
  local tools=( $( echo "${1}" ) )
  local name app
  local missing=""
  
  for i in "${tools[@]}"; do
    if [[ $i =~ .*:.* ]]; then
      name=${i#*:}
      app=${i%:*}
    else
      name=$i
      app=$i
    fi

    if $( which "$app" >/dev/null 2>&1 ); then
      doPrintStatus "├ $name" "${green}" "Found"
    else
      doPrintStatus "├ $name" "${red}" "Not found"
      missing="$missing $name"
    fi
  done

  if [ "$missing" != "" ]; then
    doPrintProgress "Installing missing tools"
    sudo apt-get install "$missing"
  fi

  echo;
}

checkMissingPackages () {
  local check_packages=('yasm' 'pkg-config' 'make' 'git' 'svn' 'cmake' 'gcc' 'autoconf' 'automake' 'cvs' 'flex' 'bison' 'makeinfo' 'g++' 'hg' 'unzip' 'patch' 'wget' 'xz' 'nasm')
  # libtool check is wonky...
  if [[ $OSTYPE == darwin* ]]; then 
    check_packages+=(glibtoolize) # homebrew special :|
  else
    check_packages+=(libtoolize) # the rest of the world
  fi

  for package in "${check_packages[@]}"; do
    if $( type -P "$package" >/dev/null ); then
      doPrintStatus "├ $package" "${green}" "Found"
    else
      doPrintStatus "├ $package" "${red}" "Not found"
      missing_packages=("$package" "${missing_packages[@]}")
    fi
  done

  if [[ -n "${missing_packages[@]}" ]]; then
   ## clear
    echo "Could not find the following execs (svn is actually package subversion, makeinfo is actually package texinfo if you're missing them): ${missing_packages[@]}"
    echo 'Install the missing packages before running this script.'
    echo "for ubuntu: $ sudo apt-get install subversion wget texinfo g++ bison flex cvs yasm automake libtool autoconf gcc cmake git make pkg-config mercurial unzip nasm -y" 
    echo "for gentoo (a non ubuntu distro): same as above, but no g++, no gcc, git is dev-vcs/git, pkg-config is dev-util/pkgconfig"
    echo "for OS X (homebrew): brew install wget cvs hg yasm automake autoconf cmake hg libtool xz pkg-config nasm"
    echo "for debian: same as ubuntu, but also add libtool-bin"
    exit 1
  fi

  local out=`cmake --version` # like cmake version 2.8.7
  local version_have=`echo "$out" | cut -d " " -f 3`

  function version { echo "$@" | awk -F. '{ printf("%d%03d%03d%03d\n", $1,$2,$3,$4); }'; }

  if [[ $(version $version_have)  < $(version '2.8.10') ]]; then
    echo "your cmake version is too old $version_have wanted 2.8.10"
    exit 1
  fi

  out=`yasm --version`
  yasm_version=`echo "$out" | cut -d " " -f 2` # like 1.1.0.112
  if [[ $(version $yasm_version)  < $(version '1.2.0') ]]; then
    echo "your yasm version is too old $yasm_version wanted 1.2.0"
    exit 1
  fi
}

((extglob_set)) && shopt -u extglob
