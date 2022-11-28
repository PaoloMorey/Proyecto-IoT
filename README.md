# Proyecto-IoT

## Levantar mosquito server
```bash
docker-compose up -d
```
## Ejecutar subcriber for mosquitto

### Activar el env y actualizar
```python
conda env create -f environment.yml && conda activate sub_mosquit && make pip-tools 
```

### Correr sub

```python
python sub.py
```