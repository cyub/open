package syscall

import (
	"syscall"
	"unsafe"
)

const (
	O_RDONLY = syscall.O_RDONLY
	O_WRONLY = syscall.O_WRONLY
	O_RDWR   = syscall.O_RDWR

	O_CREAT = syscall.O_CREAT
	O_TRUNC = syscall.O_TRUNC
)

func Open(path string, mode int, perm uint32) (fd int, err error) {
	return syscall.Open(path, mode, perm)
}

func Creat(path string, mode uint32) (fd int, err error) {
	return Open(path, O_CREAT|O_WRONLY|O_TRUNC, mode)
}

func Close(fd int) error {
	return syscall.Close(fd)
}

func Read(fd int, p []byte) (n int, err error) {
	return syscall.Read(fd, p)
}

func Write(fd int, p []byte) (n int, err error) {
	return syscall.Write(fd, p)
}

func Pread(fd int, p []byte, offset int64) (n int, err error) {
	return syscall.Pread(fd, p, offset)
}

func Pwrite(fd int, p []byte, offset int64) (n int, err error) {
	return syscall.Pwrite(fd, p, offset)
}

func Readv(fd uintptr, buff [][]byte, iovecs []syscall.Iovec) (int, error) {
	var iovecLen int = 0
	for i := 0; i < len(buff); i++ {
		l := len(buff[i])
		if l == 0 {
			continue
		}
		iovecs[i].Base = &buff[i][0]
		iovecs[i].SetLen(l)
		iovecLen++
	}
	if iovecLen == 0 {
		return 0, nil
	}
	r, _, err := syscall.RawSyscall(syscall.SYS_READV, fd, uintptr(unsafe.Pointer(&iovecs[0])), uintptr(iovecLen))
	if err != 0 {
		return 0, err
	}

	return int(r), nil
}

func Writev(fd uintptr, buff [][]byte) (int, error) {
	iovecs := make([]syscall.Iovec, 0, len(buff))
	for i := 0; i < len(buff); i++ {
		l := len(buff[i])
		if l == 0 {
			continue
		}

		iovecs = append(iovecs, syscall.Iovec{
			Base: &buff[i][0],
			Len:  uint64(l),
		})
	}

	if len(iovecs) == 0 {
		return 0, nil
	}
	r, _, err := syscall.RawSyscall(syscall.SYS_WRITEV, fd, uintptr(unsafe.Pointer(&iovecs[0])), uintptr(len(iovecs)))
	if err != 0 {
		return 0, err
	}

	return int(r), nil
}
