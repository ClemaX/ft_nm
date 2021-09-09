FROM alpine

RUN apk add --no-cache bash clang make libc-dev binutils build-base

RUN adduser --disabled-password --no-create-home --home /build -s /bin/bash runner runner


COPY --chown=runner:runner . /build

USER runner
WORKDIR /build

RUN chmod +x NM_TESTER.sh && make NAME=ft_nm

SHELL ["/bin/bash", "-c"]
# ./NM_TESTER.sh
CMD ./ft_nm ft_nm

