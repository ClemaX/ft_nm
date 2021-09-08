FROM alpine

RUN apk add --no-cache clang make libc-dev binutils build-base

RUN adduser --disabled-password --no-create-home --home /build runner runner

COPY --chown=runner:runner . /build

USER runner
WORKDIR /build

RUN make NAME=ft_nm

CMD ./ft_nm ft_nm

