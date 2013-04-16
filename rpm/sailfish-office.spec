Name: sailfish-office
Version: 0.1
Release: 1
Summary: Sailfish office viewer
Group: System/Base
License: GPLv2
Source0: %{name}-%{version}.tar.gz
BuildRequires: pkgconfig(QtDeclarative)
BuildRequires: pkgconfig(qdeclarative-boostable)
BuildRequires: pkgconfig(QtSparql)
BuildRequires: pkgconfig(QtWebKit)
BuildRequires: pkgconfig(libjollasignonuiservice)
BuildRequires: pkgconfig(mapplauncherd-qt-devel)
BuildRequires: cmake
Requires: calligra-components
Requires: sailfishsilica


%description
%{summary}.

%files
%defattr(-,root,root,-)
%{_bindir}/*
%{_datadir}/applications/*.desktop
%{_datadir}/%{name}/
%{_datadir}/dbus-1/services/org.sailfish.documents.service

%prep
%setup -q -n %{name}-%{version}


%build
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make %{?_smp_mflags}


%install
make DESTDIR=%{buildroot} install
