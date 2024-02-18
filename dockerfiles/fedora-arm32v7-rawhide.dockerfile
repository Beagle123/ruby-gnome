FROM arm32v7/fedora:rawhide

RUN \
  dnf install -y \
    adwaita-icon-theme \
    dbus-daemon \
    gcc \
    gcc-c++ \
    gdk-pixbuf2-modules \
    gnome-icon-theme \
    gnumeric \
    gstreamer1-plugins-good \
    make \
    redhat-rpm-config \
    ruby-devel \
    sudo \
    which \
    util-linux \
    xorg-x11-server-Xvfb && \
  dnf clean all

RUN \
  gem install bundler

RUN \
  useradd --user-group --create-home ruby-gnome

RUN \
  echo "ruby-gnome ALL=(ALL:ALL) NOPASSWD:ALL" | \
    EDITOR=tee visudo -f /etc/sudoers.d/ruby-gnome

USER ruby-gnome
WORKDIR /home/ruby-gnome
