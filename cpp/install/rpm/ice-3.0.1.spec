%define _unpackaged_files_terminate_build 0

%define core_arches %{ix86} x86_64

Summary: The Ice base runtime and services
Name: ice
Version: 3.0.1
Release: 1
License: GPL
Group:System Environment/Libraries
Vendor: ZeroC, Inc
URL: http://www.zeroc.com/
Source0: http://www.zeroc.com/download/Ice/3.0/Ice-%{version}.tar.gz
Source1: http://www.zeroc.com/download/Ice/3.0/IceJ-%{version}.tar.gz
Source2: http://www.zeroc.com/download/Ice/3.0/IcePy-%{version}.tar.gz
Source3: http://www.zeroc.com/download/Ice/3.0/IceCS-%{version}.tar.gz
Source4: http://www.zeroc.com/download/Ice/3.0/Ice-%{version}-demos.tar.gz
Source5: http://www.zeroc.com/download/Ice/3.0/README.Linux-RPM
Source6: http://www.zeroc.com/download/Ice/3.0/ice.ini
Source7: http://www.zeroc.com/download/Ice/3.0/configure.5.0.4.gz
Source8: http://www.zeroc.com/download/Ice/3.0/php-5.0.4.tar.bz2
Source9: http://www.zeroc.com/download/Ice/3.0/IcePHP-%{version}.tar.gz
Source10: http://www.zeroc.com/download/Ice/3.0/iceproject.xml

BuildRoot: /var/tmp/Ice-3.0.1-1-buildroot

%ifarch noarch
BuildRequires: mono-core >= 1.1.9
BuildRequires: mono-devel >= 1.1.9
%endif

BuildRequires: python >= 2.4.1
BuildRequires: python-devel >= 2.4.1
BuildRequires: bzip2-devel >= 1.0.2
BuildRequires: bzip2-libs >= 1.0.2
BuildRequires: expat-devel >= 1.9
BuildRequires: expat >= 1.9
BuildRequires: libstdc++ >= 3.2
BuildRequires: gcc >= 3.2
BuildRequires: gcc-c++ >= 3.2
BuildRequires: tar
BuildRequires: binutils >= 2.10
BuildRequires: openssl >= 0.9.7f
BuildRequires: openssl-devel >= 0.9.7f
BuildRequires: readline >= 5.0
BuildRequires: ncurses >= 5.4

%ifarch x86_64
%define icelibdir lib64
%else
%define icelibdir lib
%endif

Provides: ice-%{_arch}
%description
Ice is a modern alternative to object middleware
such as CORBA or COM/DCOM/COM+.  It is easy to learn, yet provides a
powerful network infrastructure for demanding technical applications. It
features an object-oriented specification language, easy to use C++,
Java, Python, PHP, C#, and Visual Basic mappings, a highly efficient
protocol, asynchronous method invocation and dispatch, dynamic transport
plug-ins, TCP/IP and UDP/IP support, SSL-based security, a firewall
solution, and much more.
%prep
%setup -n Ice-%{version} -q -T -D -b 0
#
# The Ice make system does not allow the prefix directory to be specified
# through an environment variable or a command line option.  So we edit some
# files in place with sed.
#
sed -i -e 's/^prefix.*$/prefix = $\(RPM_BUILD_ROOT\)/' $RPM_BUILD_DIR/Ice-%{version}/config/Make.rules
%setup -q -n IceJ-%{version} -T -D -b 1
%setup -q -n IcePy-%{version} -T -D -b 2
sed -i -e 's/^prefix.*$/prefix = $\(RPM_BUILD_ROOT\)/' $RPM_BUILD_DIR/IcePy-%{version}/config/Make.rules
%setup -q -n IceCS-%{version} -T -D -b 3 
sed -i -e 's/^prefix.*$/prefix = $\(RPM_BUILD_ROOT\)/' $RPM_BUILD_DIR/IceCS-%{version}/config/Make.rules.cs

%setup -q -n Ice-%{version}-demos -T -D -b 4 
cd $RPM_BUILD_DIR
tar xfz $RPM_SOURCE_DIR/IcePHP-%{version}.tar.gz
tar xfj $RPM_SOURCE_DIR/php-5.0.4.tar.bz2
rm -f $RPM_BUILD_DIR/php-5.0.4/ext/ice
ln -s $RPM_BUILD_DIR/IcePHP-%{version}/src/ice $RPM_BUILD_DIR/php-5.0.4/ext
cp $RPM_SOURCE_DIR/ice.ini $RPM_BUILD_DIR/IcePHP-%{version}
gzip -dc $RPM_SOURCE_DIR/configure.5.0.4.gz > $RPM_BUILD_DIR/php-5.0.4/configure

%build

cd $RPM_BUILD_DIR/Ice-%{version}
gmake OPTIMIZE=yes RPM_BUILD_ROOT=$RPM_BUILD_ROOT
cd $RPM_BUILD_DIR/IcePy-%{version}
gmake  OPTIMIZE=yes ICE_HOME=$RPM_BUILD_DIR/Ice-%{version} RPM_BUILD_ROOT=$RPM_BUILD_ROOT
cd $RPM_BUILD_DIR/IceCS-%{version}
export PATH=$RPM_BUILD_DIR/Ice-%{version}/bin:$PATH
export LD_LIBRARY_PATH=$RPM_BUILD_DIR/Ice-%{version}/lib:$LD_LIBRARY_PATH
gmake OPTIMIZE=yes ICE_HOME=$RPM_BUILD_DIR/Ice-%{version} RPM_BUILD_ROOT=$RPM_BUILD_ROOT
cd $RPM_BUILD_DIR/php-5.0.4
./configure --with-ice=shared,$RPM_BUILD_DIR/Ice-%{version}
sed -i -e 's/^EXTRA_CXXFLAGS.*$/EXTRA_CXXFLAGS = -DCOMPILE_DL_ICE/' $RPM_BUILD_DIR/php-5.0.4/Makefile
gmake

%install

rm -rf $RPM_BUILD_ROOT
cd $RPM_BUILD_DIR/Ice-%{version}
gmake RPM_BUILD_ROOT=$RPM_BUILD_ROOT install
if test ! -d $RPM_BUILD_ROOT/lib ; 
then 
    mkdir $RPM_BUILD_ROOT/lib
fi
cp -p $RPM_BUILD_DIR/IceJ-%{version}/lib/Ice.jar $RPM_BUILD_ROOT/lib/Ice.jar
cp -p $RPM_BUILD_DIR/IceJ-%{version}/lib/IceGridGUI.jar $RPM_BUILD_ROOT/lib/IceGridGUI.jar
cp -pR $RPM_BUILD_DIR/IceJ-%{version}/ant $RPM_BUILD_ROOT
cd $RPM_BUILD_DIR/IcePy-%{version}
gmake ICE_HOME=$RPM_BUILD_DIR/Ice-%{version} RPM_BUILD_ROOT=$RPM_BUILD_ROOT install
cd $RPM_BUILD_DIR/IceCS-%{version}
export PATH=$RPM_BUILD_DIR/Ice-%{version}/bin:$PATH
export LD_LIBRARY_PATH=$RPM_BUILD_DIR/Ice-%{version}/lib:$LD_LIBRARY_PATH
gmake NOGAC=yes ICE_HOME=$RPM_BUILD_DIR/Ice-%{version} RPM_BUILD_ROOT=$RPM_BUILD_ROOT install
cp $RPM_SOURCE_DIR/README.Linux-RPM $RPM_BUILD_ROOT/README
cp $RPM_SOURCE_DIR/ice.ini $RPM_BUILD_ROOT/ice.ini
cp $RPM_BUILD_DIR/php-5.0.4/modules/ice.so $RPM_BUILD_ROOT/lib/icephp.so
cp -pR $RPM_BUILD_DIR/Ice-%{version}-demos/config $RPM_BUILD_ROOT
cp $RPM_SOURCE_DIR/iceproject.xml $RPM_BUILD_ROOT/config
if test ! -d $RPM_BUILD_ROOT/%{icelibdir}/pkgconfig ; 
then 
    mkdir $RPM_BUILD_ROOT/%{icelibdir}/pkgconfig
fi

for f in icecs glacier2cs iceboxcs icegridcs icepatch2cs icestormcs; 
do 
    cp $RPM_BUILD_DIR/IceCS-%{version}/bin/$f.dll $RPM_BUILD_ROOT/bin
    cp $RPM_BUILD_DIR/IceCS-%{version}/lib/pkgconfig/$f.pc $RPM_BUILD_ROOT/%{icelibdir}/pkgconfig 
done

#
# The following commands transform a standard Ice installation directory
# structure to a directory structure more suited to integrating into a
# Linux system.
#

# Rule 1
mkdir -p $RPM_BUILD_ROOT/etc/php.d
mv $RPM_BUILD_ROOT/ice.ini $RPM_BUILD_ROOT/etc/php.d/ice.ini
# Rule 3
mkdir -p $RPM_BUILD_ROOT/usr
mv $RPM_BUILD_ROOT/%{icelibdir} $RPM_BUILD_ROOT/usr/%{icelibdir}
# Rule 1
if test -d $RPM_BUILD_ROOT/lib;
then
    mv $RPM_BUILD_ROOT/lib $RPM_BUILD_ROOT/usr
fi

mkdir -p $RPM_BUILD_ROOT/usr/lib/php/modules
mv $RPM_BUILD_ROOT/usr/lib/icephp.so $RPM_BUILD_ROOT/usr/lib/php/modules/icephp.so
# Rule 1
mkdir -p $RPM_BUILD_ROOT/usr/lib/Ice-3.0.1
mv $RPM_BUILD_ROOT/usr/lib/Ice.jar $RPM_BUILD_ROOT/usr/lib/Ice-3.0.1/Ice.jar
# Rule 1
mkdir -p $RPM_BUILD_ROOT/usr/lib/Ice-3.0.1
mv $RPM_BUILD_ROOT/usr/lib/IceGridGUI.jar $RPM_BUILD_ROOT/usr/lib/Ice-3.0.1/IceGridGUI.jar
# Rule 1
mkdir -p $RPM_BUILD_ROOT/usr/lib/mono/gac/icecs/3.0.1.0__1f998c50fec78381
mv $RPM_BUILD_ROOT/bin/icecs.dll $RPM_BUILD_ROOT/usr/lib/mono/gac/icecs/3.0.1.0__1f998c50fec78381/icecs.dll
# Rule 1
mkdir -p $RPM_BUILD_ROOT/usr/lib/mono/gac/glacier2cs/3.0.1.0__1f998c50fec78381
mv $RPM_BUILD_ROOT/bin/glacier2cs.dll $RPM_BUILD_ROOT/usr/lib/mono/gac/glacier2cs/3.0.1.0__1f998c50fec78381/glacier2cs.dll
# Rule 1
mkdir -p $RPM_BUILD_ROOT/usr/lib/mono/gac/iceboxcs/3.0.1.0__1f998c50fec78381
mv $RPM_BUILD_ROOT/bin/iceboxcs.dll $RPM_BUILD_ROOT/usr/lib/mono/gac/iceboxcs/3.0.1.0__1f998c50fec78381/iceboxcs.dll
# Rule 1
mkdir -p $RPM_BUILD_ROOT/usr/lib/mono/gac/icegridcs/3.0.1.0__1f998c50fec78381
mv $RPM_BUILD_ROOT/bin/icegridcs.dll $RPM_BUILD_ROOT/usr/lib/mono/gac/icegridcs/3.0.1.0__1f998c50fec78381/icegridcs.dll
# Rule 1
mkdir -p $RPM_BUILD_ROOT/usr/lib/mono/gac/icepatch2cs/3.0.1.0__1f998c50fec78381
mv $RPM_BUILD_ROOT/bin/icepatch2cs.dll $RPM_BUILD_ROOT/usr/lib/mono/gac/icepatch2cs/3.0.1.0__1f998c50fec78381/icepatch2cs.dll
# Rule 1
mkdir -p $RPM_BUILD_ROOT/usr/lib/mono/gac/icestormcs/3.0.1.0__1f998c50fec78381
mv $RPM_BUILD_ROOT/bin/icestormcs.dll $RPM_BUILD_ROOT/usr/lib/mono/gac/icestormcs/3.0.1.0__1f998c50fec78381/icestormcs.dll
# Rule 3
mkdir -p $RPM_BUILD_ROOT/usr/lib/Ice-3.0.1
mv $RPM_BUILD_ROOT/ant $RPM_BUILD_ROOT/usr/lib/Ice-3.0.1/ant
# Rule 3
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/Ice-3.0.1
mv $RPM_BUILD_ROOT/config $RPM_BUILD_ROOT/usr/share/doc/Ice-3.0.1/config
# Rule 3
mkdir -p $RPM_BUILD_ROOT/usr/share
mv $RPM_BUILD_ROOT/slice $RPM_BUILD_ROOT/usr/share/slice
# Rule 3
mkdir -p $RPM_BUILD_ROOT/usr
mv $RPM_BUILD_ROOT/bin $RPM_BUILD_ROOT/usr/bin
# Rule 3
mkdir -p $RPM_BUILD_ROOT/usr
mv $RPM_BUILD_ROOT/include $RPM_BUILD_ROOT/usr/include
# Rule 3
mkdir -p $RPM_BUILD_ROOT/usr/lib/Ice-3.0.1
mv $RPM_BUILD_ROOT/python $RPM_BUILD_ROOT/usr/lib/Ice-3.0.1/python
# Rule 3
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/Ice-3.0.1
mv $RPM_BUILD_ROOT/doc $RPM_BUILD_ROOT/usr/share/doc/Ice-3.0.1/doc
# Rule 1
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/Ice-3.0.1
mv $RPM_BUILD_ROOT/README $RPM_BUILD_ROOT/usr/share/doc/Ice-3.0.1/README
# Rule 1
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/Ice-3.0.1
mv $RPM_BUILD_ROOT/ICE_LICENSE $RPM_BUILD_ROOT/usr/share/doc/Ice-3.0.1/ICE_LICENSE
# Rule 1
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/Ice-3.0.1
mv $RPM_BUILD_ROOT/LICENSE $RPM_BUILD_ROOT/usr/share/doc/Ice-3.0.1/LICENSE
#
# Extract the contents of the demo packaged into the installed location.
#
mkdir -p $RPM_BUILD_ROOT/usr/share/doc/Ice-%{version}
tar xfz $RPM_SOURCE_DIR/Ice-%{version}-demos.tar.gz -C $RPM_BUILD_ROOT/usr/share/doc
cp -pR $RPM_BUILD_ROOT/usr/share/doc/Ice-%{version}-demos/* $RPM_BUILD_ROOT/usr/share/doc/Ice-%{version}
sed -i.bak -e 's/^\(src_build.*\)$/\# \1/' $RPM_BUILD_ROOT/usr/share/doc/Ice-%{version}/config/Make.rules.cs
rm -rf $RPM_BUILD_ROOT/usr/share/doc/Ice-%{version}-demos

%clean

%changelog
* Tue Nov 15 2005 ZeroC Staff
- See source distributions or the ZeroC website for more information
  about the changes in this release





%ifarch %{core_arches}
%package c++-devel
Summary: Tools and demos for developing Ice applications in C++
Group: Development/Tools
Requires: ice = 3.0.1
Requires: ice-%{_arch}
%description c++-devel
Ice is a modern alternative to object middleware
such as CORBA or COM/DCOM/COM+.  It is easy to learn, yet provides a
powerful network infrastructure for demanding technical applications. It
features an object-oriented specification language, easy to use C++,
Java, Python, PHP, C#, and Visual Basic mappings, a highly efficient
protocol, asynchronous method invocation and dispatch, dynamic transport
plug-ins, TCP/IP and UDP/IP support, SSL-based security, a firewall
solution, and much more.
%endif




%ifarch %{core_arches}
%package csharp-devel
Summary: Tools and demos for developing Ice applications in C#
Group: Development/Tools
Requires: ice-dotnet = 3.0.1
Requires: ice-%{_arch}
%description csharp-devel
Ice is a modern alternative to object middleware
such as CORBA or COM/DCOM/COM+.  It is easy to learn, yet provides a
powerful network infrastructure for demanding technical applications. It
features an object-oriented specification language, easy to use C++,
Java, Python, PHP, C#, and Visual Basic mappings, a highly efficient
protocol, asynchronous method invocation and dispatch, dynamic transport
plug-ins, TCP/IP and UDP/IP support, SSL-based security, a firewall
solution, and much more.
%endif




%ifarch %{core_arches} 
%package java-devel
Summary: Tools and demos for developing Ice applications in Java
Group: Development/Tools
Requires: ice-java = 3.0.1
Requires: ice-%{_arch}
%description java-devel
Ice is a modern alternative to object middleware
such as CORBA or COM/DCOM/COM+.  It is easy to learn, yet provides a
powerful network infrastructure for demanding technical applications. It
features an object-oriented specification language, easy to use C++,
Java, Python, PHP, C#, and Visual Basic mappings, a highly efficient
protocol, asynchronous method invocation and dispatch, dynamic transport
plug-ins, TCP/IP and UDP/IP support, SSL-based security, a firewall
solution, and much more.
%endif




%ifarch %{core_arches} 
%package python
Summary: The Ice runtime for Python applications
Group: System Environment/Libraries
Requires: ice = 3.0.1, python >= 2.4.1
Requires: ice-%{_arch}
%description python
Ice is a modern alternative to object middleware
such as CORBA or COM/DCOM/COM+.  It is easy to learn, yet provides a
powerful network infrastructure for demanding technical applications. It
features an object-oriented specification language, easy to use C++,
Java, Python, PHP, C#, and Visual Basic mappings, a highly efficient
protocol, asynchronous method invocation and dispatch, dynamic transport
plug-ins, TCP/IP and UDP/IP support, SSL-based security, a firewall
solution, and much more.
%endif




%ifarch %{core_arches}
%package python-devel
Summary: Tools and demos for developing Ice applications in Python
Group: Development/Tools
Requires: ice-python = 3.0.1
Requires: ice-%{_arch}
%description python-devel
Ice is a modern alternative to object middleware
such as CORBA or COM/DCOM/COM+.  It is easy to learn, yet provides a
powerful network infrastructure for demanding technical applications. It
features an object-oriented specification language, easy to use C++,
Java, Python, PHP, C#, and Visual Basic mappings, a highly efficient
protocol, asynchronous method invocation and dispatch, dynamic transport
plug-ins, TCP/IP and UDP/IP support, SSL-based security, a firewall
solution, and much more.
%endif




%ifarch %{core_arches}
%package php
Summary: The Ice runtime for PHP applications
Group: System Environment/Libraries
Requires: ice = 3.0.1, php = 5.0.4
Requires: ice-%{_arch}
%description php
Ice is a modern alternative to object middleware
such as CORBA or COM/DCOM/COM+.  It is easy to learn, yet provides a
powerful network infrastructure for demanding technical applications. It
features an object-oriented specification language, easy to use C++,
Java, Python, PHP, C#, and Visual Basic mappings, a highly efficient
protocol, asynchronous method invocation and dispatch, dynamic transport
plug-ins, TCP/IP and UDP/IP support, SSL-based security, a firewall
solution, and much more.
%endif




%ifarch %{core_arches}
%package php-devel
Summary: Demos for developing Ice applications in PHP
Group: Development/Tools
Requires: ice = 3.0.1, php = 5.0.4, ice-php = 3.0.1
Requires: ice-%{_arch}
%description php-devel
Ice is a modern alternative to object middleware
such as CORBA or COM/DCOM/COM+.  It is easy to learn, yet provides a
powerful network infrastructure for demanding technical applications. It
features an object-oriented specification language, easy to use C++,
Java, Python, PHP, C#, and Visual Basic mappings, a highly efficient
protocol, asynchronous method invocation and dispatch, dynamic transport
plug-ins, TCP/IP and UDP/IP support, SSL-based security, a firewall
solution, and much more.
%endif




%ifarch noarch
%package java
Summary: The Ice runtime for Java
Group: System Environment/Libraries
Requires: ice = 3.0.1, db4-java >= 4.3.27
%description java
Ice is a modern alternative to object middleware
such as CORBA or COM/DCOM/COM+.  It is easy to learn, yet provides a
powerful network infrastructure for demanding technical applications. It
features an object-oriented specification language, easy to use C++,
Java, Python, PHP, C#, and Visual Basic mappings, a highly efficient
protocol, asynchronous method invocation and dispatch, dynamic transport
plug-ins, TCP/IP and UDP/IP support, SSL-based security, a firewall
solution, and much more.
%endif




%ifarch noarch
%package dotnet
Summary: The Ice runtime for C# applications
Group: System Environment/Libraries
Requires: ice = 3.0.1, mono-core >= 1.1.9
%description dotnet
Ice is a modern alternative to object middleware
such as CORBA or COM/DCOM/COM+.  It is easy to learn, yet provides a
powerful network infrastructure for demanding technical applications. It
features an object-oriented specification language, easy to use C++,
Java, Python, PHP, C#, and Visual Basic mappings, a highly efficient
protocol, asynchronous method invocation and dispatch, dynamic transport
plug-ins, TCP/IP and UDP/IP support, SSL-based security, a firewall
solution, and much more.
%endif



%files

%ifnarch noarch
%defattr(644, root, root, 755)

%dir /usr/share/doc/Ice-3.0.1
/usr/share/doc/Ice-3.0.1/ICE_LICENSE
/usr/share/doc/Ice-3.0.1/LICENSE
/usr/share/doc/Ice-3.0.1/README
%attr(755, root, root) /usr/bin/dumpdb
%attr(755, root, root) /usr/bin/transformdb
%attr(755, root, root) /usr/bin/glacier2router
%attr(755, root, root) /usr/bin/icebox
%attr(755, root, root) /usr/bin/iceboxadmin
%attr(755, root, root) /usr/bin/icecpp
%attr(755, root, root) /usr/bin/icepatch2calc
%attr(755, root, root) /usr/bin/icepatch2client
%attr(755, root, root) /usr/bin/icepatch2server
%attr(755, root, root) /usr/bin/icestormadmin
%attr(755, root, root) /usr/bin/slice2docbook
%attr(755, root, root) /usr/bin/icegridadmin
%attr(755, root, root) /usr/bin/icegridnode
%attr(755, root, root) /usr/bin/icegridregistry
%attr(755, root, root) /usr/%{icelibdir}/libFreeze.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libFreeze.so.30
%attr(755, root, root) /usr/%{icelibdir}/libGlacier2.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libGlacier2.so.30
%attr(755, root, root) /usr/%{icelibdir}/libIceBox.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libIceBox.so.30
%attr(755, root, root) /usr/%{icelibdir}/libIcePatch2.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libIcePatch2.so.30
%attr(755, root, root) /usr/%{icelibdir}/libIce.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libIce.so.30
%attr(755, root, root) /usr/%{icelibdir}/libIceSSL.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libIceSSL.so.30
%attr(755, root, root) /usr/%{icelibdir}/libIceStormService.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libIceStormService.so.30
%attr(755, root, root) /usr/%{icelibdir}/libIceStorm.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libIceStorm.so.30
%attr(755, root, root) /usr/%{icelibdir}/libIceUtil.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libIceUtil.so.30
%attr(755, root, root) /usr/%{icelibdir}/libIceXML.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libIceXML.so.30
%attr(755, root, root) /usr/%{icelibdir}/libSlice.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libSlice.so.30
%attr(755, root, root) /usr/%{icelibdir}/libIceGrid.so.3.0.1
%attr(755, root, root) /usr/%{icelibdir}/libIceGrid.so.30
/usr/lib/Ice-3.0.1/IceGridGUI.jar
/usr/share/slice
/usr/share/doc/Ice-3.0.1/doc
%dir /usr/share/doc/Ice-3.0.1/certs
/usr/share/doc/Ice-3.0.1/certs/cacert.pem
/usr/share/doc/Ice-3.0.1/certs/c_dh1024.pem
/usr/share/doc/Ice-3.0.1/certs/client_sslconfig.xml
/usr/share/doc/Ice-3.0.1/certs/server_sslconfig.xml
/usr/share/doc/Ice-3.0.1/certs/c_rsa1024_priv.pem
/usr/share/doc/Ice-3.0.1/certs/c_rsa1024_pub.pem
/usr/share/doc/Ice-3.0.1/certs/s_dh1024.pem
/usr/share/doc/Ice-3.0.1/certs/s_rsa1024_priv.pem
/usr/share/doc/Ice-3.0.1/certs/s_rsa1024_pub.pem
/usr/share/doc/Ice-3.0.1/certs/sslconfig.dtd
/usr/share/doc/Ice-3.0.1/certs/sslconfig.xml
%dir /usr/share/doc/Ice-3.0.1/config
/usr/share/doc/Ice-3.0.1/config/templates.xml
/usr/share/doc/Ice-3.0.1/README.DEMOS


%endif

%post
%postun



%ifarch %{core_arches}
%files c++-devel
%defattr(644, root, root, 755)

%attr(755, root, root) /usr/bin/slice2cpp
%attr(755, root, root) /usr/bin/slice2freeze
/usr/include
%attr(755, root, root) /usr/%{icelibdir}/libFreeze.so
%attr(755, root, root) /usr/%{icelibdir}/libGlacier2.so
%attr(755, root, root) /usr/%{icelibdir}/libIceBox.so
%attr(755, root, root) /usr/%{icelibdir}/libIceGrid.so
%attr(755, root, root) /usr/%{icelibdir}/libIcePatch2.so
%attr(755, root, root) /usr/%{icelibdir}/libIce.so
%attr(755, root, root) /usr/%{icelibdir}/libIceSSL.so
%attr(755, root, root) /usr/%{icelibdir}/libIceStormService.so
%attr(755, root, root) /usr/%{icelibdir}/libIceStorm.so
%attr(755, root, root) /usr/%{icelibdir}/libIceUtil.so
%attr(755, root, root) /usr/%{icelibdir}/libIceXML.so
%attr(755, root, root) /usr/%{icelibdir}/libSlice.so
%dir /usr/share/doc/Ice-3.0.1
/usr/share/doc/Ice-3.0.1/demo
%attr(755, root, root) /usr/share/doc/Ice-3.0.1/demo/Freeze/backup/backup
%attr(755, root, root) /usr/share/doc/Ice-3.0.1/demo/Freeze/backup/recover
%dir /usr/share/doc/Ice-3.0.1/config
/usr/share/doc/Ice-3.0.1/config/Make.rules
/usr/share/doc/Ice-3.0.1/config/Make.rules.Linux

%post c++-devel
%postun c++-devel

%endif


%ifarch %{core_arches}
%files csharp-devel
%defattr(644, root, root, 755)

%attr(755, root, root) /usr/bin/slice2cs
%dir /usr/share/doc/Ice-3.0.1
%dir /usr/share/doc/Ice-3.0.1/config
/usr/share/doc/Ice-3.0.1/config/Make.rules.cs
/usr/%{icelibdir}/pkgconfig/icecs.pc
/usr/%{icelibdir}/pkgconfig/glacier2cs.pc
/usr/%{icelibdir}/pkgconfig/iceboxcs.pc
/usr/%{icelibdir}/pkgconfig/icegridcs.pc
/usr/%{icelibdir}/pkgconfig/icepatch2cs.pc
/usr/%{icelibdir}/pkgconfig/icestormcs.pc
/usr/share/doc/Ice-3.0.1/democs

%post csharp-devel

%ifnarch noarch

pklibdir="lib"

%ifarch x86_64
pklibdir="lib64"
%endif

for f in icecs glacier2cs iceboxcs icegridcs icepatch2cs icestormcs;
do
    sed -i.bak -e "s/^mono_root.*$/mono_root = \/usr/" /usr/$pklibdir/pkgconfig/$f.pc ; 
done
	
%endif
%postun csharp-devel

%endif


%ifarch %{core_arches}
%files java-devel
%defattr(644, root, root, 755)

%attr(755, root, root) /usr/bin/slice2java
%attr(755, root, root) /usr/bin/slice2freezej
%dir /usr/lib/Ice-3.0.1
/usr/lib/Ice-3.0.1/ant
%dir /usr/share/doc/Ice-3.0.1
%dir /usr/share/doc/Ice-3.0.1/certs
/usr/share/doc/Ice-3.0.1/certs/certs.jks
/usr/share/doc/Ice-3.0.1/certs/client.jks
/usr/share/doc/Ice-3.0.1/certs/server.jks
%dir /usr/share/doc/Ice-3.0.1/config
/usr/share/doc/Ice-3.0.1/config/build.properties
/usr/share/doc/Ice-3.0.1/config/common.xml
/usr/share/doc/Ice-3.0.1/config/iceproject.xml
/usr/share/doc/Ice-3.0.1/demoj

%post java-devel
%postun java-devel

%endif


%ifarch %{core_arches}
%files python
%defattr(644, root, root, 755)

/usr/lib/Ice-3.0.1/python

%post python
%postun python

%endif


%ifarch %{core_arches}
%files python-devel
%defattr(644, root, root, 755)

%attr(755, root, root) /usr/bin/slice2py
%dir /usr/share/doc/Ice-3.0.1
/usr/share/doc/Ice-3.0.1/demopy

%post python-devel
%postun python-devel

%endif


%ifarch %{core_arches}
%files php
%defattr(644, root, root, 755)

%attr(755, root, root) /usr/lib/php/modules
/etc/php.d/ice.ini

%post php
%postun php

%endif


%ifarch %{core_arches}
%files php-devel
%defattr(644, root, root, 755)

/usr/share/doc/Ice-3.0.1/demophp

%post php-devel
%postun php-devel

%endif


%ifarch noarch
%files java
%defattr(644, root, root, 755)

%dir /usr/lib/Ice-3.0.1
/usr/lib/Ice-3.0.1/Ice.jar

%post java
%postun java

%endif


%ifarch noarch
%files dotnet
%defattr(644, root, root, 755)

/usr/lib/mono/gac/glacier2cs/3.0.1.0__1f998c50fec78381/glacier2cs.dll
/usr/lib/mono/gac/icecs/3.0.1.0__1f998c50fec78381/icecs.dll
/usr/lib/mono/gac/iceboxcs/3.0.1.0__1f998c50fec78381/iceboxcs.dll
/usr/lib/mono/gac/icegridcs/3.0.1.0__1f998c50fec78381/icegridcs.dll
/usr/lib/mono/gac/icepatch2cs/3.0.1.0__1f998c50fec78381/icepatch2cs.dll
/usr/lib/mono/gac/icestormcs/3.0.1.0__1f998c50fec78381/icestormcs.dll
%attr(755, root, root) /usr/bin/iceboxnet.exe

%post dotnet
%postun dotnet

%endif

