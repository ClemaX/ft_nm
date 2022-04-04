FROM alpine as builder

RUN apk add --no-cache bash clang make libc-dev binutils build-base

RUN adduser --disabled-password --no-create-home --home /build -s /bin/bash builder builder

COPY --chown=builder:builder . /build

USER builder
WORKDIR /build

RUN make NAME=ft_nm


FROM alpine

RUN adduser --disabled-password --no-create-home --home /build -s /bin/bash runner runner

USER runner

COPY --from=builder --chown=runner:runner /build/ft_nm /build/ft_nm

WORKDIR /build

CMD ./ft_nm ft_nm
