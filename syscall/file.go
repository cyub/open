package syscall

import "syscall"

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
