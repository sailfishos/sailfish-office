Name: sailfish-office
Version: 0.1
Release: 1
Summary: Sailfish office viewer
Group: System/Base
License: GPLv2
Source0: %{name}-%{version}.tar.gz
BuildRequires: pkgconfig(Qt5Declarative)
BuildRequires: pkgconfig(Qt5Quick)
BuildRequires: pkgconfig(Qt5Widgets)
BuildRequires: pkgconfig(Qt5WebKit)
BuildRequires: pkgconfig(libjollasignonuiservice-qt5)
BuildRequires: libqt5sparql-devel
#BuildRequires: poppler-qt5-devel poppler-qt5 poppler-devel poppler
BuildRequires: mapplauncherd-qt5-devel
BuildRequires: cmake
Requires: calligra-components calligra-filters
Requires: sailfishsilica-qt5
Requires: sailfish-components-accounts-qt5
Requires: libqt5sparql-tracker


%description
%{summary}.

%files
%defattr(-,root,root,-)
%{_bindir}/*
%{_libdir}/qt5/qml/Sailfish/Office/
%{_datadir}/applications/*.desktop
%{_datadir}/%{name}/
%{_datadir}/dbus-1/services/org.sailfish.documents.service
#%{_datadir}/translations/

%prep
%setup -q -n %{name}-%{version}


%build
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make %{?_smp_mflags}


%install
make DESTDIR=%{buildroot} install
