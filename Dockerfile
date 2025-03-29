FROM dirigiblelabs/dirigible:latest

COPY nst-one target/dirigible/repository/root/registry/public/nst-one

ENV DIRIGIBLE_HOME_URL=/services/web/nst-one/gen/edm/index.html

ENV DIRIGIBLE_MULTI_TENANT_MODE=false

EXPOSE 8080
