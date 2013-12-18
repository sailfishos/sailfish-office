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
#BuildRequires: pkgconfig(Qt5WebKit)
BuildRequires: pkgconfig(Qt5DBus)
BuildRequires: libqt5sparql-devel
BuildRequires: poppler-qt5-devel poppler-qt5 poppler-devel poppler
BuildRequires: mapplauncherd-qt5-devel
BuildRequires: cmake
BuildRequires: qt5-qttools-linguist
BuildRequires: libjollasignonuiservice-qt5-devel
Requires: calligra-components >= 2.7.9+git4
Requires: calligra-filters >= 2.7.9+git4
Requires: sailfishsilica-qt5
Requires: sailfish-components-accounts-qt5
Requires: libqt5sparql-tracker
#Requires: qt5-qtqml-import-webkitplugin
Requires: libjollasignonuiservice-qt5-plugin
Requires:  %{name}-all-translations

%package ts-devel
Summary: Translation source for %{name}
License: GPLv2
Group: System/Base


%description
%{summary}.

%description ts-devel
%{summary}.


%files
%defattr(-,root,root,-)
%{_bindir}/*
%{_libdir}/qt5/qml/Sailfish/Office/
%{_datadir}/applications/*.desktop
%{_datadir}/%{name}/
%{_datadir}/translations/
%{_datadir}/dbus-1/interfaces/
%{_datadir}/dbus-1/services/

%files ts-devel
%{_datadir}/translations/source/*.ts


%prep
%setup -q -n %{name}-%{version}


%build
cmake -DCMAKE_INSTALL_PREFIX=/usr .
make %{?_smp_mflags}


%install
make DESTDIR=%{buildroot} install
