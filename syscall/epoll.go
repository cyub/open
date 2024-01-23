package syscall

import "syscall"

func Eventfd2(flag int) (int, error) {
	var fd, _, err = syscall.Syscall(syscall.SYS_EVENTFD2, 0, uintptr(flag), 0)
	if err == syscall.Errno(0) {
		return int(fd), nil
	}
	return int(fd), err
}
